#include <chrono>
#include <iostream>
#include <iomanip>
#include <string>

#include "assignment_sudoku.h"

static void PrintUsage()
{
    std::cout << "Qlik Sudoku Assignment\n\n";
    std::cout << "Usage:\n";
    std::cout << "  sudoku <file>              # load and print puzzle\n";
    std::cout << "  sudoku print <file>        # load and print puzzle\n";
    std::cout << "  sudoku solve <file>        # solve and print puzzle\n";
}

int main(int argc, char **argv)
{
    if (argc < 2)
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
    else if (command != "print" && command != "load")
    {
        std::cerr << "Error: Unknown command '" << command << "'.\n\n";
        PrintUsage();
        return 1;
    }
    else
    {
        std::cout << SudokuToPrettyString(board) << '\n';
    }
    return 0;
}
