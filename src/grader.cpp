#include "assignment_sudoku.h"

#include "solver_core.h"

namespace {
AssignmentDifficulty ClassifyBySearchEffort(const AssignmentSolveStats &stats) {
    // Search-effort fallback once pair-based techniques are involved.
    if (stats.backtracks <= 10 && stats.branches <= 5 && stats.maxDepth <= 3) {
        return AssignmentDifficulty::HARD;
    }
    return AssignmentDifficulty::SAMURAI;
}
} // namespace

std::optional<AssignmentDifficulty> GradeSudoku(
    const AssignmentSolveResult &metrics) {
    if (!metrics.validInput) {
        return std::nullopt;
    }
    if (!metrics.solved || metrics.solutionCount == 0) {
        return std::nullopt;
    }
    if (metrics.solutionCount > 1) {
        return std::nullopt;
    }

    if (!metrics.techniques.usedBacktracking && metrics.techniques.hiddenSingles == 0 &&
        metrics.techniques.nakedPairs == 0 && metrics.techniques.hiddenPairs == 0) {
        return AssignmentDifficulty::EASY;
    }
    if (!metrics.techniques.usedBacktracking && metrics.techniques.hiddenSingles > 0 &&
        metrics.techniques.nakedPairs == 0 && metrics.techniques.hiddenPairs == 0) {
        return AssignmentDifficulty::MEDIUM;
    }
    if (metrics.techniques.nakedPairs > 0 || metrics.techniques.hiddenPairs > 0) {
        return AssignmentDifficulty::HARD;
    }

    // Fall back to search effort bands from the previous grading rule.
    if (ClassifyBySearchEffort(metrics.stats) == AssignmentDifficulty::HARD) {
        return ClassifyBySearchEffort(metrics.stats);
    }

    return AssignmentDifficulty::SAMURAI;
}
