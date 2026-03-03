#include "sudoku_rules.h"

#include <array>
#include <cstdint>

namespace sudoku {

namespace {
inline int BoxIndex(int row, int col) { return (row / 3) * 3 + (col / 3); }

inline uint16_t DigitToBit(int digit) {
  return static_cast<uint16_t>(1u << (digit - 1));
}
} // namespace

bool IsBoardConsistent(const Board &board) {
  std::array<uint16_t, 9> rowUsed{};
  std::array<uint16_t, 9> colUsed{};
  std::array<uint16_t, 9> boxUsed{};

  for (int row = 0; row < 9; ++row) {
    for (int col = 0; col < 9; ++col) {
      const int value = board.at(row, col);
      if (value == 0)
        continue;
      if (value < 1 || value > 9)
        return false;

      const uint16_t bit = DigitToBit(value);
      const int box = BoxIndex(row, col);
      if ((rowUsed[row] & bit) || (colUsed[col] & bit) || (boxUsed[box] & bit))
        return false;
      rowUsed[row] |= bit;
      colUsed[col] |= bit;
      boxUsed[box] |= bit;
    }
  }
  return true;
}

} // namespace sudoku
