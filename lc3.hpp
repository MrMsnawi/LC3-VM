#ifndef LC3_HPP
#define LC3_HPP

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/termios.h>

using u16 = uint16_t;

constexpr size_t MEMORY_MAX = 1 << 16;

enum {
	MR_KBSR = 0xFE00,  // Keyboard status register
	MR_KBDR = 0xFE02   // Keyboard data register
};

enum {
	R_R0 = 0,
	R_R1,
	R_R2,
	R_R3,
	R_R4,
	R_R5,
	R_R6,
	R_R7,
	R_PC,
	R_COND,
	R_COUNT
};

enum {
	FL_POS = 1 << 0,
	FL_ZRO = 1 << 1,
	FL_NEG = 1 << 2
};

enum {
	OP_BR = 0,
	OP_ADD,
	OP_LD,
	OP_ST,
	OP_JSR,
	OP_AND,
	OP_LDR,
	OP_STR,
	OP_RTI,
	OP_NOT,
	OP_LDI,
	OP_STI,
	OP_JMP,
	OP_RES,
	OP_LEA,
	OP_TRAP
};

class LC3 {
private:
	u16 reg[R_COUNT];
	u16 memory[MEMORY_MAX];
	int running;
	bool input_buffering_enabled;

	u16     mem_read(u16 address);
	void    mem_write(u16 address, u16 val);
	void    disable_input_buffering();
	void    restore_input_buffering();
	u16     check_key();
	u16     sign_extend(u16 x, int bit_count);
	void    update_flags(u16 r);

public:
	LC3();
	~LC3();
	bool	load_image(const char* image_path);
	void    run();
};

#endif
