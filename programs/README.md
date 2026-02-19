# LC-3 Programs

This directory contains pre-assembled LC-3 programs that can be executed by the LC-3 VM.

## Programs

### 2048.obj (2.3 KB)
A port of the popular 2048 game to LC-3 assembly.

**How to run:**
```bash
cd ..
./lc3-vm programs/2048.obj
```

**Controls:**
- **W** - Move up
- **A** - Move left
- **S** - Move down
- **D** - Move right
- **Ctrl+C** - Exit

**Gameplay:**
- Combine numbered tiles to reach the 2048 tile
- Each move, a new tile (2 or 4) appears on a random empty square
- When you can't move anymore, the game ends
- Try to achieve the highest score!

### rogue.obj (762 B)
A roguelike dungeon crawler game written in LC-3 assembly.

**How to run:**
```bash
cd ..
./lc3-vm programs/rogue.obj
```

**Controls:**
- **WASD** - Move around the dungeon
- **Ctrl+C** - Exit

**Gameplay:**
- Explore a randomly generated dungeon
- Fight monsters and find treasures
- Navigate through challenging rooms

## Running the Programs

From the project root directory, use:

```bash
# Run 2048
./lc3-vm programs/2048.obj

# Run Rogue
./lc3-vm programs/rogue.obj
```

## File Format

These are LC-3 object files (.obj) which are binary files containing:
1. **Origin** (first 16-bit word) - Starting memory address (0x3000)
2. **Instructions/Data** - The program code and any embedded data

The VM automatically:
- Reads the origin address
- Loads the program at the specified address
- Handles endianness conversion (big-endian to little-endian)
- Executes the program

## Program Details

| Program | Size | Description |
|---------|------|-------------|
| 2048.obj | 2.3 KB | Puzzle game - combine tiles |
| rogue.obj | 762 B | Roguelike dungeon crawler |

## Source Code

The source code for these programs is available at:
- Official LC-3 Tutorial: https://www.jmeiners.com/lc3-vm/
- GitHub repositories with LC-3 implementations

## Terminal Support

**Note:** For best experience with 2048, use an ANSI-compatible terminal:
- When prompted, enter `y` for ANSI terminal support
- This enables proper game rendering with box-drawing characters

## Troubleshooting

If a program doesn't run:

1. **Check the VM builds correctly:**
   ```bash
   cd ..
   make clean && make
   ```

2. **Verify the program file exists:**
   ```bash
   ls -l programs/*.obj
   ```

3. **Check for terminal issues:**
   - Try on a different terminal emulator
   - Some terminals may not support raw mode input properly

4. **Re-download if needed:**
   ```bash
   cd programs
   rm *.obj
   curl -L -o 2048.obj https://www.jmeiners.com/lc3-vm/supplies/2048.obj
   curl -L -o rogue.obj https://www.jmeiners.com/lc3-vm/supplies/rogue.obj
   ```

## Creating Your Own Programs

To create and run your own LC-3 programs:

1. Write your program in LC-3 assembly
2. Use the official LC-3 assembler to create a .obj file
3. Run with: `./lc3-vm your_program.obj`

LC-3 assembler download: http://highered.mheducation.com/sites/0072467509/student_view0/lc-3_simulator.html
