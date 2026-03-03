#include "utility.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

std::optional<AssignmentDifficulty> ParseDifficulty(const std::string &text) {
    if (text == "easy") {
        return AssignmentDifficulty::EASY;
    }
    if (text == "medium") {
        return AssignmentDifficulty::MEDIUM;
    }
    if (text == "hard") {
        return AssignmentDifficulty::HARD;
    }
    if (text == "samurai") {
        return AssignmentDifficulty::SAMURAI;
    }
    return std::nullopt;
}

const char *DifficultyToString(AssignmentDifficulty difficulty) {
    switch (difficulty) {
        case AssignmentDifficulty::EASY:
            return "easy";
        case AssignmentDifficulty::MEDIUM:
            return "medium";
        case AssignmentDifficulty::HARD:
            return "hard";
        case AssignmentDifficulty::SAMURAI:
            return "samurai";
        default:
            return "unknown";
    }
}

std::string BuildGeneratedPuzzlePath(AssignmentDifficulty difficulty) {
    const auto now = std::chrono::system_clock::now();
    const std::time_t t = std::chrono::system_clock::to_time_t(now);
    const auto ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif

    std::ostringstream filename;
    filename << DifficultyToString(difficulty) << "_" << std::put_time(&tm, "%Y%m%d_%H%M%S")
             << "_" << std::setfill('0') << std::setw(3) << ms.count() << ".txt";
    return "generated_puzzle/" + filename.str();
}

void PrintUsage(std::ostream &out) {
    out << "Qlik Sudoku Assignment\n\n";
    out << "Usage:\n";
    out << "  sudoku print <file>                 # load and print puzzle\n";
    out << "  sudoku solve <file>                 # solve and print puzzle\n";
    out << "  sudoku grade <file>                 # classify puzzle difficulty\n";
    out << "  sudoku generate <difficulty> [--symmetry]\n";
}

void PrintSolveReport(const AssignmentSolveResult &result, double elapsedMs, std::ostream &out) {
    out << "Solve metrics:\n";
    out << "  branches:   " << result.stats.branches << '\n';
    out << "  backtracks: " << result.stats.backtracks << '\n';
    out << "  max depth:  " << result.stats.maxDepth << '\n';
    out << "  time (ms):  " << std::fixed << std::setprecision(3) << elapsedMs << '\n';
    out << "Puzzle solved successfully:\n";
    out << SudokuToPrettyString(result.solution) << '\n';
}

void PrintGradeReport(AssignmentDifficulty difficulty, const AssignmentSolveResult &metrics,
                      std::ostream &out) {
    out << "Difficulty: " << DifficultyToString(difficulty) << '\n';
    out << "Grade metrics:\n";
    out << "  branches:   " << metrics.stats.branches << '\n';
    out << "  backtracks: " << metrics.stats.backtracks << '\n';
    out << "  max depth:  " << metrics.stats.maxDepth << '\n';
    out << "  solutions:  " << metrics.solutionCount << '\n';
    out << "  naked singles:  " << metrics.techniques.nakedSingles << '\n';
    out << "  hidden singles: " << metrics.techniques.hiddenSingles << '\n';
    out << "  naked pairs:    " << metrics.techniques.nakedPairs << '\n';
    out << "  hidden pairs:   " << metrics.techniques.hiddenPairs << '\n';
}

void PrintGenerateReport(const std::string &outputPath, AssignmentDifficulty target,
                         AssignmentDifficulty detected, std::ostream &out) {
    out << "Generated puzzle saved to: " << outputPath << '\n';
    out << "Target difficulty:   " << DifficultyToString(target) << '\n';
    out << "Detected difficulty: " << DifficultyToString(detected) << '\n';
}
