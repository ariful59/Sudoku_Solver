#ifndef SOLVER_ENGINE_H
#define SOLVER_ENGINE_H

#include "sudoku.h"

namespace sudoku::internal {
  SolveResult SolveWithConfig(const Board& input, bool requireUnique = true,
    bool useHumanTechniques = false);
} // namespace sudoku::internal

#endif
