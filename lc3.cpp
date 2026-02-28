#include "lc3.hpp"

struct termios original_tio;

LC3::LC3()
{
    running = 0;
    input_buffering_enabled = false;
    std::memset(reg, 0, sizeof(reg));
    std::memset(memory, 0, sizeof(memory));
    reg[R_PC] = 0x3000;
    reg[R_COND] = FL_ZRO;
}

LC3::~LC3()
{
    restore_input_buffering();
}

static u16 swap16(u16 x)
{
    return (x << 8) | (x >> 8); // swap high and low bytes
}

void LC3::disable_input_buffering()
{
    if (tcgetattr(STDIN_FILENO, &original_tio) == -1)
    {
        return;
    }

    struct termios new_tio = original_tio;
    new_tio.c_lflag &= ~ICANON & ~ECHO;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &new_tio) == 0)
    {
        input_buffering_enabled = true;
    }
}

void LC3::restore_input_buffering()
{
    if (input_buffering_enabled)
    {
        tcsetattr(STDIN_FILENO, TCSANOW, &original_tio);
        input_buffering_enabled = false;
    }
}

u16 LC3::check_key()
{
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    return select(1, &readfds, NULL, NULL, &timeout) > 0;
}

u16 LC3::mem_read(u16 address)
{
    if (address == MR_KBSR)
    {
        if (check_key())
        {
            memory[MR_KBSR] = (1 << 15);
            memory[MR_KBDR] = getchar();
        }
        else
        {
            memory[MR_KBSR] = 0;
        }
    }
    return memory[address];
}

void LC3::mem_write(u16 address, u16 val)
{
    memory[address] = val;
}

bool LC3::load_image(const char* image_path)
{
    FILE* file = fopen(image_path, "rb");
    if (!file) {
        return false;
    }

    // Read origin address (first 16-bit word)
    u16 origin;
    if (fread(&origin, sizeof(origin), 1, file) != 1) {
        fclose(file);
        return false;
    }
    origin = swap16(origin);  // Convert from big-endian

    // Read program into memory starting at origin
    size_t max_read = MEMORY_MAX - static_cast<size_t>(origin);
    u16* p = memory + origin;
    size_t read = fread(p, sizeof(u16), max_read, file);

    // Swap all words from big-endian to little-endian
    while (read-- > 0) {
        *p = swap16(*p);
        p++;
    }

    fclose(file);
    return true;
}

u16 LC3::sign_extend(u16 x, int bit_count) {
    if((x >> (bit_count - 1)) & 1) {
        x |= (0xFFFF << bit_count);
    }
    return x;
}

void LC3::update_flags(u16 r)
{
    if (reg[r] == 0) {
        reg[R_COND] = FL_ZRO;
    } else if (reg[r] >> 15) { // check sign bit (bit 15)
        reg[R_COND] = FL_NEG;
    } else {
        reg[R_COND] = FL_POS;
    }
}

void LC3::run()
{
    running = 1;
    disable_input_buffering();

    while (running)
    {
        // Fetch
        u16 instr = mem_read(reg[R_PC]++);

        // Decode
        u16 op = instr >> 12; // Top 4 bits

        // Execute
        switch (op) {
            case OP_BR: {
                u16 cond_flag = (instr >> 9) & 0x7; // bits 11-9
                u16 pc_offset = sign_extend(instr & 0x1FF, 9);
                if (cond_flag & reg[R_COND]) { // cjeck if any of n/z/p match current flags
                    reg[R_PC] += pc_offset; // Jump
                }
                break;
            }
            case OP_ADD: {
                // Extract fields from instructino
                u16 dr = (instr >> 9) & 0x7; // bits 11-9 (destinatino)
                u16 sr1 = (instr >> 6) & 0x7; // bits 8-6 (source 1)
                u16 imm_flag = (instr >> 5) & 0x1; // bit 5 (mode)

                if (imm_flag)
                {
                    // immediate mode
                    u16 imm5 = sign_extend(instr & 0x1F, 5); // bits 4-0, sign-extend
                    reg[dr] = reg[sr1] + imm5;
                } else {
                    // register mode
                    u16 sr2 = instr & 0x7; // bits 2-0
                    reg[dr] = reg[sr1] + reg[sr2];
                }
                // update conditino flags
                update_flags(dr);
                break;
            }
            case OP_LD:
            {
                u16 dr = (instr >> 9) & 0x7;
                u16 pc_offset = sign_extend(instr & 0x1FF, 9); // bits 8-0
                reg[dr] = mem_read(reg[R_PC] + pc_offset);
                update_flags(dr);
                break;
            }
            case OP_ST:
            {
                u16 sr = (instr >> 9) & 0x7;
                u16 pc_offset = sign_extend(instr & 0x1FF, 9);
                mem_write(reg[R_PC] + pc_offset, reg[sr]);
                break;
            }
            case OP_JSR: {
                reg[R_R7] = reg[R_PC];
                u16 base_r = (instr >> 6) & 0x7;
                u16 pc_offset = sign_extend(instr & 0x7FF, 11);
                if ((instr >> 11) & 0x1)
                {
                    reg[R_PC] += pc_offset;
                } else {
                    reg[R_PC] = reg[base_r];
                }
                break;
            }
            case OP_AND:
            {
                u16 dr = (instr >> 9) & 0x7;
                u16 sr1 = (instr >> 6) & 0x7;
                u16 imm_flag = (instr >> 5) & 0x1;

                if (imm_flag)
                {
                    u16 imm5 = sign_extend(instr & 0x1F, 5);
                    reg[dr] = reg[sr1] & imm5;
                } else {
                    u16 sr2 = instr & 0x7;
                    reg[dr] = reg[sr1] & reg[sr2];
                }
                update_flags(dr);
                break;
            }
            case OP_LDR: {
                u16 dr = (instr >> 9) & 0x7;
                u16 base_r = (instr >> 6) & 0x7;
                u16 offset = sign_extend(instr & 0x3F, 6); // 6bits
                reg[dr] = mem_read(reg[base_r] + offset);
                update_flags(dr);
                break;
            }
            case OP_STR: {
                u16 sr = (instr >> 9) & 0x7;
                u16 base_r = (instr >> 6) & 0x7;
                u16 offset = sign_extend(instr & 0x3F, 6);
                mem_write(reg[base_r] + offset, reg[sr]);
                break;
            }
            case OP_RTI: break;
            case OP_NOT:
            {
                u16 dr = (instr >> 9) & 0x7;
                u16 sr = (instr >> 6) & 0x7;
                reg[dr] = ~reg[sr]; // bitwise NOT
                update_flags(dr);
                break;
            }
            case OP_LDI: {
                u16 dr = (instr >> 9) & 0x7;
                u16 pc_offset = sign_extend(instr & 0x1FF, 9);
                u16 addr = mem_read(reg[R_PC] + pc_offset); // read first address
                reg[dr] = mem_read(addr); // read from that address
                update_flags(dr);
                break;
            }
            case OP_STI: {
                u16 sr = (instr >> 9) & 0x7;
                u16 pc_offset = sign_extend(instr & 0x1FF, 9);
                u16 addr = mem_read(reg[R_PC] + pc_offset); // read address
                mem_write(addr, reg[sr]);
                break;
            }
            case OP_JMP: {
                u16 base_r = (instr >> 6) & 0x7;
                reg[R_PC] = reg[base_r]; // jump to address in register
                break;
            }
            case OP_RES: break;
            case OP_LEA: {
                u16 dr = (instr >> 9) & 0x7;
                u16 pc_offset = sign_extend(instr & 0x1FF, 9);
                reg[dr] = reg[R_PC] + pc_offset; // store address, not data
                update_flags(dr);
                break;
            }
            case OP_TRAP: {
                u16 trap_vect = instr & 0xFF; // extract 8 bits trap vector
                reg[R_R7] = reg[R_PC];

                switch (trap_vect) {
                    case 0x20: // GETC - read a character from keyboard
                        reg[R_R0] = (u16)getchar();
                        update_flags(R_R0);
                        break;
                    case 0x21: // OUT - output character in R0
                        putchar((char)reg[R_R0]);
                        fflush(stdout);
                        break;
                    case 0x22: // PUTS - output null terminated string
                    {
                        u16 addr = reg[R_R0];
                        u16 c = mem_read(addr);
                        while (c) {
                            putchar((char)c);
                            c = mem_read(++addr);
                        }
                        fflush(stdout);
                    }
                    break;
                    case 0x23: // IN - input character with prompt
                        printf("Enter a character: ");
                        reg[R_R0] = (u16)getchar();
                        putchar((char)reg[R_R0]);
                        fflush(stdout);
                        update_flags(R_R0);
                        break;
                    case 0x24: // PUTSP - output packed string (2 chars per word)
                    {
                        u16 addr = reg[R_R0];
                        u16 word = mem_read(addr);
                        while (word)
                        {
                            char c1 = word & 0xFF; // Low byte
                            putchar(c1);
                            char c2 = word >> 8; // high byte
                            if (c2) putchar(c2);
                            word = mem_read(++addr);
                        }
                        fflush(stdout);
                    }
                    break;

                    case 0x25: // HAlt - stop execution
                        puts("HALT");
                        fflush(stdout);
                        running = false;
                        break;
                }
                break;
            }
            default: break;
        }
    }

    restore_input_buffering();
}
