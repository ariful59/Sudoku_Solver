#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <optional>
#include <string>

#include "assignment_sudoku.h"
#include "utility.h"

namespace {

int CheckSingleSolution(const AssignmentSolveResult &result) {
    if (!result.validInput) {
        std::cerr << "Error: Invalid input puzzle\n";
        return 1;
    }
    if (!result.solved || result.solutionCount == 0) {
        std::cerr << "Error: No solution found\n";
        return 1;
    }
    if (result.solutionCount > 1) {
        std::cerr << "Error: Puzzle has multiple solutions (not unique)\n";
        return 1;
    }
    return 0;
}

int HandleGenerate(int argc, char **argv) {
    if (argc < 3) {
        PrintUsage(std::cout);
        return 1;
    }

    const std::optional<AssignmentDifficulty> target = ParseDifficulty(argv[2]);
    if (!target.has_value()) {
        std::cerr << "Error: Unknown difficulty '" << argv[2] << "'.\n";
        std::cerr << "Allowed: easy, medium, hard, samurai\n";
        return 1;
    }

    bool useSymmetry = false;
    if (argc >= 4) {
        const std::string flag = argv[3];
        if (flag == "--symmetry") {
            useSymmetry = true;
        } else {
            std::cerr << "Error: Unknown generate option '" << flag << "'.\n";
            std::cerr << "Supported option: --symmetry\n";
            return 1;
        }
    }
    if (argc > 4) {
        PrintUsage(std::cout);
        return 1;
    }

    const std::optional<AssignmentGeneratedPuzzle> generated =
        GenerateSudoku(*target, useSymmetry);
    if (!generated.has_value()) {
        std::cerr << "Error: Could not generate puzzle.\n";
        return 1;
    }

    std::filesystem::create_directories("generated_puzzle");
    const std::string outputPath = BuildGeneratedPuzzlePath(*target);

    std::string ioError;
    if (!SaveSudokuToTextFile(outputPath, generated->puzzle, ioError)) {
        std::cerr << "Error: " << ioError << '\n';
        return 1;
    }

    PrintGenerateReport(outputPath, *target, generated->detectedDifficulty, std::cout);
    return 0;
}

int LoadBoard(int argc, char **argv, AssignmentSudoku &board) {
    if (argc < 3) {
        PrintUsage(std::cout);
        return 1;
    }

    std::string error;
    if (!LoadSudokuFromTextFile(argv[2], board, error)) {
        std::cerr << "Error: " << error << '\n';
        return 1;
    }
    return 0;
}

int HandlePrint(const AssignmentSudoku &board) {
    std::cout << SudokuToPrettyString(board) << '\n';
    return 0;
}

int HandleSolve(const AssignmentSudoku &board) {
    const auto start = std::chrono::steady_clock::now();
    const AssignmentSolveResult result = SolveSudokuUnique(board);
    const auto end = std::chrono::steady_clock::now();

    const int validation = CheckSingleSolution(result);
    if (validation != 0) {
        return validation;
    }

    const auto elapsedUs =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    const double elapsedMs = static_cast<double>(elapsedUs) / 1000.0;
    PrintSolveReport(result, elapsedMs, std::cout);
    return 0;
}

int HandleGrade(const AssignmentSudoku &board) {
    const int solutionLimit = 2;
    const AssignmentSolveResult metrics = SolveSudokuUnique(board, solutionLimit, true);
    const int validation = CheckSingleSolution(metrics);
    if (validation != 0) {
        return validation;
    }

    const std::optional<AssignmentDifficulty> difficulty = GradeSudoku(metrics);
    if (!difficulty.has_value()) {
        std::cerr << "Error: Unable to classify puzzle difficulty\n";
        return 1;
    }

    PrintGradeReport(*difficulty, metrics, std::cout);
    return 0;
}

} // namespace

int main(int argc, char **argv) {
    if (argc < 2) {
        PrintUsage(std::cout);
        return 1;
    }

    const std::string command = argv[1];

    if (command == "generate") {
        return HandleGenerate(argc, argv);
    }

    AssignmentSudoku board{};
    const int loadStatus = LoadBoard(argc, argv, board);
    if (loadStatus != 0) {
        return loadStatus;
    }

    if (command == "print") {
        return HandlePrint(board);
    }
    if (command == "solve") {
        return HandleSolve(board);
    }
    if (command == "grade") {
        return HandleGrade(board);
    }

    std::cerr << "Error: Unknown command '" << command << "'.\n\n";
    PrintUsage(std::cout);
    return 1;
}
