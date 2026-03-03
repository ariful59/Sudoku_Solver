#ifndef SOLVER_CORE_H
#define SOLVER_CORE_H

#include "assignment_sudoku.h"

namespace assignment_internal
{

    AssignmentSolveResult SolveWithConfig(const AssignmentSudoku &input,
                                          int solutionLimit = 2,
                                          bool useHumanTechniques = false);

} // namespace assignment_internal

#endif
