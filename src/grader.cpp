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

std::optional<AssignmentDifficulty> GradeSudoku(const AssignmentSudoku &input,
                                                AssignmentSolveResult &outMetrics) {
    outMetrics = assignment_internal::SolveWithConfig(input, 2, true);

    if (!outMetrics.validInput) {
        return std::nullopt;
    }
    if (!outMetrics.solved || outMetrics.solutionCount == 0) {
        return std::nullopt;
    }
    if (outMetrics.solutionCount > 1) {
        return std::nullopt;
    }

    if (!outMetrics.techniques.usedBacktracking && outMetrics.techniques.hiddenSingles == 0 &&
        outMetrics.techniques.nakedPairs == 0 && outMetrics.techniques.hiddenPairs == 0) {
        return AssignmentDifficulty::EASY;
    }
    if (!outMetrics.techniques.usedBacktracking && outMetrics.techniques.hiddenSingles > 0 &&
        outMetrics.techniques.nakedPairs == 0 && outMetrics.techniques.hiddenPairs == 0) {
        return AssignmentDifficulty::MEDIUM;
    }
    if (outMetrics.techniques.nakedPairs > 0 || outMetrics.techniques.hiddenPairs > 0) {
        return AssignmentDifficulty::HARD;
    }

    // Fall back to search effort bands from the previous grading rule.
    if (ClassifyBySearchEffort(outMetrics.stats) == AssignmentDifficulty::HARD) {
        return ClassifyBySearchEffort(outMetrics.stats);
    }

    return AssignmentDifficulty::SAMURAI;
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
