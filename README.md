# Sudoku Solver

## Overview
This project is a Sudoku Solver using C++ that can:
1. Load and print Sudoku puzzles from text files.
2. Solve puzzles and verify uniqueness (`exactly one` solution).
3. Grade puzzle difficulty (`easy`, `medium`, `hard`, `samurai`).
4. Generate unique puzzles with approximate target difficulty with symmetric option.

## Prerequisites
1. CMake 3.16+
2. A C++17 compiler (AppleClang, Clang, GCC, or MSVC)
3. If CMake is missing, install it from [cmake.org/download](https://cmake.org/download/) or your system package manager.

## Get the Code
```bash
git clone git@github.com:ariful59/Sudoku_Solver.git
cd Sudoku_Solver
```

## Build (macOS / Linux / Windows)
From the project root directory:

```bash
cmake -S . -B build
cmake --build build -j
```

Executable path:
- `./build/sudoku`

Executable paths (Windows / Visual Studio):
- `.\build\Debug\sudoku.exe` 

## Quick Start (macOS / Linux)
```bash
# Print puzzle from file
./build/sudoku print ./assignment_examples/hard.txt

# Solve puzzle (must be uniquely solvable)
./build/sudoku solve ./assignment_examples/medium.txt

# Grade puzzle difficulty
./build/sudoku grade ./assignment_examples/medium.txt

# Generate puzzle by target difficulty
./build/sudoku generate easy

# Generate puzzle with optional 180-degree symmetry
./build/sudoku generate hard --symmetry
```

## Quick Start (Windows)
```powershell
# Print puzzle from file
.\build\Debug\sudoku.exe print .\assignment_examples\hard.txt

# Solve puzzle (must be uniquely solvable)
.\build\Debug\sudoku.exe solve .\assignment_examples\samurai.txt

# Grade puzzle difficulty
.\build\Debug\sudoku.exe grade .\assignment_examples\medium.txt

# Generate puzzle by target difficulty
.\build\Debug\sudoku.exe generate easy

.\build\Debug\sudoku.exe generate samurai

# Generate puzzle with optional 180-degree symmetry
.\build\Debug\sudoku.exe generate hard --symmetry
```

Generated puzzles are printed as well as saved automatically in:
- `generated_sudokus/<difficulty>_YYYYMMDD_HHMMSS_mmm.txt`

## Commands
1. `sudoku print <file>`
2. `sudoku solve <file>`
3. `sudoku grade <file>`
4. `sudoku generate <difficulty> [--symmetry]`

Allowed difficulty values:
- `easy`, `medium`, `hard`, `samurai`

Detailed design and workflows are documented in:
- [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md)
