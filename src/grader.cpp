#include "sudoku.h"

namespace sudoku
{

  std::optional<Difficulty> GradeSudoku(const Board& board,
    SolveResult& metrics)
  {
    const bool requireUnique = false;
    const bool useHumanTechniques = true;
    metrics = SolveSudoku(board, requireUnique, useHumanTechniques);
    return GradePuzzleDifficulty(metrics);
  }

  std::optional<Difficulty> GradePuzzleDifficulty(const SolveResult& metrics)
  {
    const auto& t = metrics.techniques;
    const auto& s = metrics.stats;

    const bool backtracking = t.usedBacktracking;
    const bool usedHiddenSingles = t.hiddenSingles > 0;
    const bool usedNakedSingles = t.nakedSingles >= 0;
    const bool usedPairTechniques = (t.nakedPairs > 0 || t.hiddenPairs > 0);
    const bool limitedSearch =
      (s.backtracks <= 10 && s.branches <= 5 && s.maxDepth <= 3);

    if (!metrics.validInput || !metrics.solved || metrics.solutionCount != 1)
      return std::nullopt;

    // Easy: solved with singles only, no backtracking.
    if (!backtracking && !usedPairTechniques && !usedHiddenSingles && usedNakedSingles)
      return Difficulty::EASY;

    // Medium: hidden singles used, no pair techniques/backtracking.
    if (!backtracking && !usedPairTechniques && usedHiddenSingles)
      return Difficulty::MEDIUM;

    // Hard: pair techniques used, or limited search effort.
    if (usedPairTechniques || limitedSearch)
      return Difficulty::HARD;

    // Samurai: heavier search required.
    return Difficulty::SAMURAI;
  }

} // namespace sudoku
