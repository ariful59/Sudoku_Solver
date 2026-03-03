# Qlik Sudoku Assignment

## Overview
This project provides:
1. Part 1: Read a Sudoku puzzle from a text file and print it.
2. Part 2: Solve the Sudoku and verify that the solution is unique.

The executable is a console program named `sudoku`.

## Prerequisites
1. CMake 3.16 or newer
2. A C++17 compiler (AppleClang/Clang/GCC/MSVC)

## Build
From the project root:

```bash
cmake -S . -B build
cmake --build build -j
```

## Run
### Part 1: Load + print puzzle
```bash

./build/sudoku print ./assignment_examples/easy.txt
```

### Part 2: Solve + uniqueness check
```bash
./build/sudoku solve ./assignment_examples/easy.txt
./build/sudoku solve ./assignment_examples/hard.txt
```

### Difficulty grading
```bash
./build/sudoku grade ./assignment_examples/easy.txt
./build/sudoku grade ./assignment_examples/samurai.txt
```

`grade` validates uniqueness and classifies difficulty as `easy`, `medium`, `hard`, or `samurai`.

Behavior in `solve` mode:
1. Prints solved board only if exactly one solution exists.
2. Returns error if no solution exists.
3. Returns error if multiple solutions exist.
4. Prints solver diagnostics:
   - branch count
   - backtrack count
   - max DFS depth
   - elapsed solve time in milliseconds
