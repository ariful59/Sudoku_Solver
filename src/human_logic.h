#ifndef HUMAN_LOGIC_H
#define HUMAN_LOGIC_H

#include "board_state.h"

namespace sudoku::internal {

void RunHumanTechniques(BoardState &state, TechniqueStats &techniques);

} // namespace sudoku::internal

#endif
