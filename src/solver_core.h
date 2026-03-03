#ifndef SOLVER_CORE_H
#define SOLVER_CORE_H

#include "sudoku.h"

namespace sudoku::internal {

SolveResult SolveWithConfig(const Board &input, int solutionLimit = 2,
                            bool useHumanTechniques = false);

} // namespace sudoku::internal

#endif
