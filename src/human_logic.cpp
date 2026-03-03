#include "human_logic.h"

#include <array>
#include <vector>

namespace sudoku::internal {
  namespace {
    template <typename Fn> bool ApplyToUnits(const BoardState& state, Fn&& fn) {
      for (int row = 0; row < 9; ++row) {
        std::vector<int> unit;
        for (int col = 0; col < 9; ++col)
          if (state.board[row * 9 + col] == 0)
            unit.push_back(row * 9 + col);
        if (fn(unit))
          return true;
      }
      for (int col = 0; col < 9; ++col) {
        std::vector<int> unit;
        for (int row = 0; row < 9; ++row)
          if (state.board[row * 9 + col] == 0)
            unit.push_back(row * 9 + col);
        if (fn(unit))
          return true;
      }
      for (int box = 0; box < 9; ++box) {
        const int br = (box / 3) * 3, bc = (box % 3) * 3;
        std::vector<int> unit;
        for (int dr = 0; dr < 3; ++dr) {
          for (int dc = 0; dc < 3; ++dc) {
            const int row = br + dr, col = bc + dc;
            if (state.board[row * 9 + col] == 0)
              unit.push_back(row * 9 + col);
          }
        }
        if (fn(unit))
          return true;
      }
      return false;
    }

    bool ApplyNakedSingles(BoardState& state, TechniqueStats& techniques) {
      bool progress = false, changed = true;
      while (changed) {
        changed = false;
        for (int row = 0; row < 9; ++row) {
          for (int col = 0; col < 9; ++col) {
            if (state.board[row * 9 + col] != 0)
              continue;
            const uint16_t mask = state.candidates(row, col);
            if (Popcount(mask) == 1 && state.place(row, col, BitToDigit(mask))) {
              ++techniques.nakedSingles;
              changed = progress = true;
            }
          }
        }
      }
      return progress;
    }

    bool ApplyHiddenSingles(BoardState& state, TechniqueStats& techniques) {
      bool progress = false;
      for (int row = 0; row < 9; ++row) {
        std::array<int, 9> count{}, posCol{};
        for (int col = 0; col < 9; ++col) {
          if (state.board[row * 9 + col] != 0)
            continue;
          const uint16_t mask = state.candidates(row, col);
          for (int d = 1; d <= 9; ++d)
            if (mask & DigitBit(d)) {
              ++count[d - 1];
              posCol[d - 1] = col;
            }
        }
        for (int d = 1; d <= 9; ++d)
          if (count[d - 1] == 1 && state.place(row, posCol[d - 1], d)) {
            ++techniques.hiddenSingles;
            progress = true;
          }
      }
      for (int col = 0; col < 9; ++col) {
        std::array<int, 9> count{}, posRow{};
        for (int row = 0; row < 9; ++row) {
          if (state.board[row * 9 + col] != 0)
            continue;
          const uint16_t mask = state.candidates(row, col);
          for (int d = 1; d <= 9; ++d)
            if (mask & DigitBit(d)) {
              ++count[d - 1];
              posRow[d - 1] = row;
            }
        }
        for (int d = 1; d <= 9; ++d)
          if (count[d - 1] == 1 && state.place(posRow[d - 1], col, d)) {
            ++techniques.hiddenSingles;
            progress = true;
          }
      }
      for (int box = 0; box < 9; ++box) {
        const int br = (box / 3) * 3, bc = (box % 3) * 3;
        std::array<int, 9> count{}, posRow{}, posCol{};
        for (int dr = 0; dr < 3; ++dr) {
          for (int dc = 0; dc < 3; ++dc) {
            const int row = br + dr, col = bc + dc;
            if (state.board[row * 9 + col] != 0)
              continue;
            const uint16_t mask = state.candidates(row, col);
            for (int d = 1; d <= 9; ++d)
              if (mask & DigitBit(d)) {
                ++count[d - 1];
                posRow[d - 1] = row;
                posCol[d - 1] = col;
              }
          }
        }
        for (int d = 1; d <= 9; ++d)
          if (count[d - 1] == 1 && state.place(posRow[d - 1], posCol[d - 1], d)) {
            ++techniques.hiddenSingles;
            progress = true;
          }
      }
      return progress;
    }

    bool ApplyNakedPairs(BoardState& state, TechniqueStats& techniques) {
      auto ScanUnit = [&](const std::vector<int>& unit) -> bool {
        for (size_t i = 0; i < unit.size(); ++i) {
          const int idx1 = unit[i];
          const uint16_t m1 = state.candidates(idx1 / 9, idx1 % 9);
          if (Popcount(m1) != 2)
            continue;
          for (size_t j = i + 1; j < unit.size(); ++j) {
            const int idx2 = unit[j];
            const uint16_t m2 = state.candidates(idx2 / 9, idx2 % 9);
            if (m1 != m2)
              continue;
            for (int idx : unit) {
              if (idx == idx1 || idx == idx2 || state.board[idx] != 0)
                continue;
              const uint16_t mask = state.candidates(idx / 9, idx % 9);
              if (!(mask & m1))
                continue;
              const uint16_t remaining = static_cast<uint16_t>(mask & ~m1);
              // Naked-pair elimination would normally prune candidates.
              // With current BoardState API, we only place when pruning yields a single.
              if (Popcount(remaining) == 1 &&
                state.place(idx / 9, idx % 9, BitToDigit(remaining))) {
                ++techniques.nakedPairs;
                return true;
              }
            }
          }
        }
        return false;
        };
      return ApplyToUnits(state, ScanUnit);
    }

    bool ApplyHiddenPairs(BoardState& state, TechniqueStats&) {
      auto ScanUnit = [&](const std::vector<int>& unit) -> bool {
        std::array<uint16_t, 9> appears{};
        for (int d = 1; d <= 9; ++d) {
          uint16_t occ = 0;
          for (size_t i = 0; i < unit.size(); ++i)
            // record the positions where digit d is a candidate
            if (state.candidates(unit[i] / 9, unit[i] % 9) & DigitBit(d))
              occ |= (1u << i);
          appears[d - 1] = occ;
        }
        for (int d1 = 1; d1 <= 9; ++d1) {
          if (Popcount(appears[d1 - 1]) != 2)
            continue;
          for (int d2 = d1 + 1; d2 <= 9; ++d2) {
            if (appears[d1 - 1] != appears[d2 - 1])
              continue;
            //todo
            // Hidden pairs need candidate-mask pruning; with current BoardState API this is a no-op for now.
            break;
          }
        }
        return false;
        };
      return ApplyToUnits(state, ScanUnit);
    }

  } // namespace

  void RunHumanTechniques(BoardState& state, TechniqueStats& techniques) {
    bool progress = true;
    while (progress && !state.solved()) {
      progress = false;
      if (ApplyNakedSingles(state, techniques)) {
        progress = true;
        continue;
      }
      if (ApplyHiddenSingles(state, techniques)) {
        progress = true;
        continue;
      }
      if (ApplyNakedPairs(state, techniques)) {
        progress = true;
        continue;
      }
      if (ApplyHiddenPairs(state, techniques)) {
        progress = true;
        continue;
      }
    }
  }
} // namespace sudoku::internal
