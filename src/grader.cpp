#include "sudoku.h"

#include "solver_core.h"

namespace sudoku {

namespace {
Difficulty ClassifyBySearchEffort(const SolveStats &stats) {
  if (stats.backtracks <= 10 && stats.branches <= 5 && stats.maxDepth <= 3)
    return Difficulty::HARD;
  return Difficulty::SAMURAI;
}
} // namespace

std::optional<Difficulty> GradeSudoku(const SolveResult &metrics) {
  if (!metrics.validInput || !metrics.solved || metrics.solutionCount != 1)
    return std::nullopt;

  if (!metrics.techniques.usedBacktracking &&
      metrics.techniques.hiddenSingles == 0 &&
      metrics.techniques.nakedPairs == 0 && metrics.techniques.hiddenPairs == 0)
    return Difficulty::EASY;

  if (!metrics.techniques.usedBacktracking &&
      metrics.techniques.hiddenSingles > 0 &&
      metrics.techniques.nakedPairs == 0 && metrics.techniques.hiddenPairs == 0)
    return Difficulty::MEDIUM;

  if (metrics.techniques.nakedPairs > 0 || metrics.techniques.hiddenPairs > 0)
    return Difficulty::HARD;

  return ClassifyBySearchEffort(metrics.stats);
}

} // namespace sudoku
