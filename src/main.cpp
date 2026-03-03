#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>

#include "assignment_sudoku.h"

static void PrintUsage()
{
    std::cout << "Qlik Sudoku Assignment\n\n";
    std::cout << "Usage:\n";
    std::cout << "  sudoku print <file>        # load and print puzzle\n";
    std::cout << "  sudoku solve <file>        # solve and print puzzle\n";
    std::cout << "  sudoku grade <file>        # classify puzzle difficulty\n";
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        PrintUsage();
        return 1;
    }

    const bool hasCommand = (argc >= 3);
    const std::string command = hasCommand ? argv[1] : "print";
    const std::string inputPath = hasCommand ? argv[2] : argv[1];

    AssignmentSudoku board{};
    std::string error;
    if (!LoadSudokuFromTextFile(inputPath, board, error))
    {
        std::cerr << "Error: " << error << '\n';
        return 1;
    }

    if (command == "solve")
    {
        const auto start = std::chrono::steady_clock::now();
        AssignmentSolveResult result = SolveSudokuUnique(board);
        const auto end = std::chrono::steady_clock::now();

        if (!result.validInput)
        {
            std::cerr << "Error: Invalid input puzzle\n";
            return 1;
        }
        if (!result.solved || result.solutionCount == 0)
        {
            std::cerr << "Error: No solution found\n";
            return 1;
        }
        if (result.solutionCount > 1)
        {
            std::cerr << "Error: Puzzle has multiple solutions (not unique)\n";
            return 1;
        }

        board = result.solution;
        const auto elapsedUs =
            std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        const double elapsedMs = static_cast<double>(elapsedUs) / 1000.0;

        std::cout << "Solve metrics:\n";
        std::cout << "  branches:   " << result.stats.branches << '\n';
        std::cout << "  backtracks: " << result.stats.backtracks << '\n';
        std::cout << "  max depth:  " << result.stats.maxDepth << '\n';
        std::cout << "  time (ms):  " << std::fixed << std::setprecision(3) << elapsedMs
                  << '\n';
        std::cout << "Puzzle solved successfully:\n";
        std::cout << SudokuToPrettyString(board) << '\n';
    }

    else if (command == "grade")
    {
        AssignmentSolveResult metrics{};
        const std::optional<AssignmentDifficulty> difficulty = GradeSudoku(board, metrics);
        if (!difficulty.has_value())
        {
            std::cerr << "Error: Unable to grade puzzle (invalid, unsolved, or non-unique)\n";
            return 1;
        }

        std::cout << "Difficulty: " << DifficultyToString(*difficulty) << '\n';
        std::cout << "Grade metrics:\n";
        std::cout << "  branches:   " << metrics.stats.branches << '\n';
        std::cout << "  backtracks: " << metrics.stats.backtracks << '\n';
        std::cout << "  max depth:  " << metrics.stats.maxDepth << '\n';
        std::cout << "  solutions:  " << metrics.solutionCount << '\n';
        std::cout << "  naked singles:  " << metrics.techniques.nakedSingles << '\n';
        std::cout << "  hidden singles: " << metrics.techniques.hiddenSingles << '\n';
        std::cout << "  naked pairs:    " << metrics.techniques.nakedPairs << '\n';
        std::cout << "  hidden pairs:   " << metrics.techniques.hiddenPairs << '\n';

         std::cout << SudokuToPrettyString(metrics.solution) << '\n';
    }
    else if (command != "print" && command != "solve" && command != "grade")
    {
        std::cerr << "Error: Unknown command '" << command << "'.\n\n";
        PrintUsage();
        return 1;
    }
    return 0;
}
