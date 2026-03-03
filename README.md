# Qlik Sudoku Assignment

## Overview
This project is a C++ Sudoku CLI that can:
1. Load and print Sudoku puzzles from text files.
2. Solve puzzles and verify uniqueness (`exactly one` solution).
3. Grade puzzle difficulty (`easy`, `medium`, `hard`, `samurai`).
4. Generate new puzzles with approximate target difficulty (optionally symmetric).

The executable is `sudoku`.

## Prerequisites
1. CMake 3.16+
2. A C++17 compiler (AppleClang, Clang, GCC, or MSVC)

## Build
From the project root directory:

```bash
cmake -S . -B build
cmake --build build
```

If build succeeds, the executable will be available at:
- `./build/sudoku`

## Quick Start
```bash
# Print a puzzle
./build/sudoku print ./assignment_examples/easy.txt

# Solve and validate uniqueness
./build/sudoku solve ./assignment_examples/easy.txt

# Grade difficulty
./build/sudoku grade ./assignment_examples/easy.txt

# Generate a puzzle
./build/sudoku generate easy

# Generate a puzzle with rotational symmetry
./build/sudoku generate hard --symmetry
```

Generated puzzles are saved automatically in:
- `generated_puzzle/<difficulty>_YYYYMMDD_HHMMSS_mmm.txt`

## Commands
1. `sudoku print <file>`
2. `sudoku solve <file>`
3. `sudoku grade <file>`
4. `sudoku generate <difficulty> [--symmetry]`

Allowed difficulty values:
- `easy`, `medium`, `hard`, `samurai`

## Project Structure
```text
Qlik_Sudoku_Assignment/
├── include/
│   └── assignment_sudoku.h    # Public data types and API declarations.
├── src/
│   ├── main.cpp               # CLI entry point and command routing.
│   ├── puzzle_io.cpp          # Puzzle file load/save and board pretty-print.
│   ├── sudoku_rules.cpp       # Sudoku rule validation (IsBoardConsistent).
│   ├── board_state.h          # Internal mutable board state and candidate masks.
│   ├── board_state.cpp        # Internal mutable board state and candidate masks.
│   ├── solver_core.h          # Core DFS solver and public solving APIs.
│   ├── solver_core.cpp        # Core DFS solver and public solving APIs.
│   ├── human_logic.h          # Human-style deterministic techniques.
│   ├── human_logic.cpp        # Human-style deterministic techniques.
│   ├── grader.cpp             # Difficulty classification from solver/human metrics.
│   ├── generator.cpp          # Puzzle generation and uniqueness-preserving clue removal.
│   ├── utility.h              # CLI/helper utilities.
│   └── utility.cpp            # CLI/helper utilities (parsing difficulty, usage/report printing, output path naming).
└── assignment_examples/
    ├── easy.txt
    ├── medium.txt
    ├── hard.txt
    ├── samurai.txt
    ├── escargot.txt
    ├── pro.txt
    ├── invalid.txt
    └── multiple.txt
```

## How It Works
### Solver
The solver uses:
1. Bitmask-based candidate tracking.
2. Constraint propagation via naked singles during DFS.
3. MRV heuristic (pick cell with minimum remaining values).
4. Backtracking search with `solutionLimit` to support uniqueness checks.

Uniqueness check strategy:
- Solve with `solutionLimit = 2`
- `solutionCount == 1` means unique
- `solutionCount > 1` means multiple solutions

### Human Logic
Human-style techniques are implemented in `human_logic.cpp` and tracked in metrics:
1. Naked singles
2. Hidden singles
3. Naked pairs
4. Hidden pairs

These metrics are used mainly for grading.

### Grading
`grade`:
1. Solves with uniqueness validation.
2. Uses technique counters and search effort metrics to map to:
   - `easy`, `medium`, `hard`, `samurai`

### Generator
`generate`:
1. Creates a solved board.
2. Removes clues while preserving unique solution.
3. Optionally removes clues in 180-degree symmetric pairs (`--symmetry`).
4. Grades generated puzzle and returns the closest match to target difficulty found during generation.

## Output and Errors
### Solve output includes
1. Branches
2. Backtracks
3. Max DFS depth
4. Runtime in milliseconds

### Common errors
1. Invalid input puzzle (rule violation)
2. No solution found
3. Multiple solutions (not unique)
4. Invalid command or invalid difficulty value

## Exit Codes
1. `0` = success
2. `1` = error

## Assumptions
1. A puzzle is valid for `solve` and `grade` only when it has exactly one solution (`solutionCount == 1`), checked with `solutionLimit = 2`.
2. Difficulty labels are implementation-based (this project’s techniques and thresholds), not a universal Sudoku standard.
3. Difficulty mapping used here:
   - `easy`: solved with naked singles only.
   - `medium`: requires hidden singles.
   - `hard`: uses pair techniques with limited search effort.
   - `samurai`: requires stronger search effort than `hard`.
4. Search effort metrics are interpreted as:
   - `branches`: number of decision points where multiple candidates were tried.
   - `backtracks`: number of times the solver had to undo and try a different path.
   - `maxDepth`: deepest recursion level reached during search.
5. The `hard` cutoff (`backtracks <= 10 && branches <= 5 && maxDepth <= 3`) is a calibration choice for this assignment dataset.
6. Symmetry mode in generation is 180-degree rotational symmetry when `--symmetry` is used. Other symmetry styles are intentionally not implemented.
7. Generation targets difficulty approximately and returns the closest match found within bounded attempts.
8. Input scope is standard 9x9 Sudoku format.
9. Generation is randomized, so produced puzzles can differ between runs.
10. For symmetry background and variants, see [SudokuWiki Introduction](https://www.sudokuwiki.org/Introduction).
