#include "sudoku.h"

#include "solver_engine.h"

namespace sudoku {

  SolveResult SolveSudoku(const Board& input, bool requireUnique,
    bool useHumanTechniques) {
    return internal::SolveWithConfig(input, requireUnique, useHumanTechniques);
  }

  bool GenerateSolvedBoard(Board& outBoard) {
    Board empty{};
    // to generate a solved board from empty we don't need uniqueness or use human techniques
    const SolveResult result =
      internal::SolveWithConfig(empty, false, false);
    if (!result.validInput || result.solutionCount != 1)
      return false;
    outBoard = result.solution;
    return true;
  }

} // namespace sudoku
