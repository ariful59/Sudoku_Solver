#include "board_state.h"

namespace sudoku::internal {

int BoxIndex(int row, int col) { return (row / 3) * 3 + (col / 3); }

uint16_t DigitBit(int digit) {
  return static_cast<uint16_t>(1u << (digit - 1));
}

int Popcount(uint16_t x) {
  int count = 0;
  while (x) {
    x &= static_cast<uint16_t>(x - 1);
    ++count;
  }
  return count;
}

int BitToDigit(uint16_t bit) {
  int digit = 1;
  while (bit > 1) {
    bit >>= 1;
    ++digit;
  }
  return digit;
}

BoardState::BoardState(const Board &input) {
  board = input.cells;
  for (int row = 0; row < 9 && valid; ++row) {
    for (int col = 0; col < 9 && valid; ++col) {
      const int value = board[row * 9 + col];
      if (value == 0)
        continue;
      if (value < 1 || value > 9) {
        valid = false;
        break;
      }
      const uint16_t bit = DigitBit(value);
      const int box = BoxIndex(row, col);
      if ((rowMask[row] & bit) || (colMask[col] & bit) ||
          (boxMask[box] & bit)) {
        valid = false;
        break;
      }
      rowMask[row] |= bit;
      colMask[col] |= bit;
      boxMask[box] |= bit;
    }
  }
}

bool BoardState::solved() const {
  for (int value : board)
    if (value == 0)
      return false;
  return true;
}

uint16_t BoardState::candidates(int row, int col) const {
  if (board[row * 9 + col] != 0)
    return 0;
  const int box = BoxIndex(row, col);
  return static_cast<uint16_t>(kAllDigitsMask &
                               ~(rowMask[row] | colMask[col] | boxMask[box]));
}

bool BoardState::place(int row, int col, int value) {
  const int idx = row * 9 + col;
  if (board[idx] != 0)
    return false;

  const uint16_t bit = DigitBit(value);
  const int box = BoxIndex(row, col);
  if ((rowMask[row] & bit) || (colMask[col] & bit) || (boxMask[box] & bit))
    return false;

  board[idx] = value;
  rowMask[row] |= bit;
  colMask[col] |= bit;
  boxMask[box] |= bit;
  return true;
}

void BoardState::remove(int row, int col, int value) {
  const int idx = row * 9 + col;
  const uint16_t bit = DigitBit(value);
  const int box = BoxIndex(row, col);
  board[idx] = 0;
  rowMask[row] &= static_cast<uint16_t>(~bit);
  colMask[col] &= static_cast<uint16_t>(~bit);
  boxMask[box] &= static_cast<uint16_t>(~bit);
}

} // namespace sudoku::internal
