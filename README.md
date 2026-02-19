# LC-3 Virtual Machine (C++)

A C++ implementation of the LC-3 (Little Computer 3) virtual machine.

This project simulates the LC-3 CPU, memory, instruction execution cycle, and terminal I/O behavior needed to run LC-3 object programs such as 2048.

## Features

- 16-bit LC-3 architecture simulation
- 65,536 memory locations
- Full fetch-decode-execute loop
- Implemented core instruction groups:
  - Arithmetic/logic: `ADD`, `AND`, `NOT`
  - Data movement: `LD`, `ST`, `LDR`, `STR`, `LDI`, `STI`, `LEA`
  - Control flow: `BR`, `JMP`, `JSR/JSRR`
  - System calls: `TRAP` routines (`GETC`, `OUT`, `PUTS`, `IN`, `PUTSP`, `HALT`)
- Memory-mapped keyboard registers (`KBSR`, `KBDR`)
- LC-3 object image loading with endianness conversion
- Terminal raw-mode input handling for interactive programs

## Project Structure

- `lc3.hpp` - VM declarations, enums, and class interface
- `lc3.cpp` - VM implementation (instructions, memory, traps, runtime loop)
- `main.cpp` - CLI entry point and image loading
- `Makefile` - build rules (`all`, `clean`, `fclean`, `re`, `run`)
- `programs/` - sample LC-3 object programs

## Build

```bash
make
```

## Run

Run with one or more LC-3 object files:

```bash
./lc3-vm programs/2048.obj
```

or

```bash
./lc3-vm <image1.obj> [image2.obj ...]
```

## Makefile Targets

- `make` / `make all` - build executable
- `make clean` - remove object files
- `make fclean` - remove object files and executable
- `make re` - full rebuild (`fclean` then `all`)
- `make run` - run executable

## Notes

- On launch of some programs (like 2048), you may be prompted with terminal capability questions.
- Controls for 2048 are typically `W`, `A`, `S`, `D`.

## Reference

This implementation is based on and inspired by the LC-3 VM article:

- https://www.jmeiners.com/lc3-vm/
