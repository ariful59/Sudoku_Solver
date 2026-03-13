#include "board_state.h"

namespace sudoku::internal {

  int BoxIndex(int row, int col) { return (row / 3) * 3 + (col / 3); }

  uint16_t DigitBit(int digit) {
    return static_cast<uint16_t>(1u << (digit - 1));
  }

  int Popcount(uint16_t x) {
    int count = 0;
    while (x) {
      //clear the lowest set bit
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

  BoardState::BoardState(const Board& input) {
    board = input.cells;
    pruneMask.fill(kAllDigitsMask);
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
    if (valid) {
      RecomputeCandidates();
    }
  }

  bool BoardState::solved() const {
    for (int value : board)
      if (value == 0)
        return false;
    return true;
  }

  uint16_t BoardState::candidates(int row, int col) const {
    return candidateMask[row * 9 + col];
  }

  bool BoardState::place(int row, int col, int value) {
    const int idx = row * 9 + col;
    if (board[idx] != 0)
      return false;

    const uint16_t bit = DigitBit(value);
    const int box = BoxIndex(row, col);
    if ((rowMask[row] & bit) || (colMask[col] & bit) || (boxMask[box] & bit))
      return false;
    if ((candidateMask[idx] & bit) == 0)
      return false;

    board[idx] = value;
    rowMask[row] |= bit;
    colMask[col] |= bit;
    boxMask[box] |= bit;
    RecomputeCandidates();
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
    RecomputeCandidates();
  }

  bool BoardState::pruneCandidates(int row, int col, uint16_t removeMask) {
    const int idx = row * 9 + col;
    if (board[idx] != 0)
      return false;
    const uint16_t before = pruneMask[idx];
    pruneMask[idx] = static_cast<uint16_t>(pruneMask[idx] & ~removeMask);
    if (pruneMask[idx] == before)
      return false;
    const int box = BoxIndex(row, col);
    const uint16_t base = static_cast<uint16_t>(kAllDigitsMask &
      ~(rowMask[row] | colMask[col] | boxMask[box]));
    candidateMask[idx] = static_cast<uint16_t>(base & pruneMask[idx]);
    return true;
  }

  void BoardState::RecomputeCandidates() {
    for (int row = 0; row < 9; ++row) {
      for (int col = 0; col < 9; ++col) {
        const int idx = row * 9 + col;
        if (board[idx] != 0) {
          candidateMask[idx] = 0;
          continue;
        }
        const int box = BoxIndex(row, col);
        const uint16_t base = static_cast<uint16_t>(kAllDigitsMask &
          ~(rowMask[row] | colMask[col] | boxMask[box]));
        candidateMask[idx] = static_cast<uint16_t>(base & pruneMask[idx]);
      }
    }
  }

} // namespace sudoku::internal
