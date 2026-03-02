#ifndef BOARD_STATE_H
#define BOARD_STATE_H

#include <array>
#include <cstdint>

#include "assignment_sudoku.h"

namespace assignment_internal {

constexpr uint16_t kAllDigitsMask = 0x1FF;

int BoxIndex(int row, int col);
uint16_t DigitBit(int digit);
int Popcount(uint16_t x);
int BitToDigit(uint16_t bit);

struct BoardState {
    std::array<int, 81> board{};
    std::array<uint16_t, 9> rowMask{};
    std::array<uint16_t, 9> colMask{};
    std::array<uint16_t, 9> boxMask{};
    bool valid = true;

    explicit BoardState(const AssignmentSudoku &input);

    bool solved() const;
    uint16_t candidates(int row, int col) const;
    bool place(int row, int col, int value);
    void remove(int row, int col, int value);
};

}  // namespace assignment_internal

#endif
