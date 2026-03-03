#include "human_logic.h"

#include <array>
#include <vector>

namespace assignment_internal {
namespace {

template <typename Fn>
bool ApplyToUnits(const BoardState &state, Fn &&fn) {
    for (int row = 0; row < 9; ++row) {
        std::vector<int> unit;
        for (int col = 0; col < 9; ++col) {
            if (state.board[row * 9 + col] == 0) {
                unit.push_back(row * 9 + col);
            }
        }
        if (fn(unit)) {
            return true;
        }
    }

    for (int col = 0; col < 9; ++col) {
        std::vector<int> unit;
        for (int row = 0; row < 9; ++row) {
            if (state.board[row * 9 + col] == 0) {
                unit.push_back(row * 9 + col);
            }
        }
        if (fn(unit)) {
            return true;
        }
    }

    for (int box = 0; box < 9; ++box) {
        const int br = (box / 3) * 3;
        const int bc = (box % 3) * 3;
        std::vector<int> unit;
        for (int dr = 0; dr < 3; ++dr) {
            for (int dc = 0; dc < 3; ++dc) {
                const int row = br + dr;
                const int col = bc + dc;
                if (state.board[row * 9 + col] == 0) {
                    unit.push_back(row * 9 + col);
                }
            }
        }
        if (fn(unit)) {
            return true;
        }
    }

    return false;
}

bool ApplyNakedSingles(BoardState &state, AssignmentTechniqueStats &techniques) {
    bool progress = false;
    bool changed = true;
    while (changed) {
        changed = false;
        for (int row = 0; row < 9; ++row) {
            for (int col = 0; col < 9; ++col) {
                if (state.board[row * 9 + col] != 0) {
                    continue;
                }
                const uint16_t mask = state.candidates(row, col);
                if (mask == 0) {
                    continue;
                }
                if (Popcount(mask) == 1) {
                    const int value = BitToDigit(mask);
                    if (state.place(row, col, value)) {
                        ++techniques.nakedSingles;
                        changed = true;
                        progress = true;
                    }
                }
            }
        }
    }
    return progress;
}

bool ApplyHiddenSingles(BoardState &state, AssignmentTechniqueStats &techniques) {
    bool progress = false;

    for (int row = 0; row < 9; ++row) {
        std::array<int, 9> count{};
        std::array<int, 9> posCol{};
        for (int col = 0; col < 9; ++col) {
            if (state.board[row * 9 + col] != 0) {
                continue;
            }
            const uint16_t mask = state.candidates(row, col);
            for (int d = 1; d <= 9; ++d) {
                if (mask & DigitBit(d)) {
                    ++count[d - 1];
                    posCol[d - 1] = col;
                }
            }
        }
        for (int d = 1; d <= 9; ++d) {
            if (count[d - 1] == 1 && state.place(row, posCol[d - 1], d)) {
                ++techniques.hiddenSingles;
                progress = true;
            }
        }
    }

    for (int col = 0; col < 9; ++col) {
        std::array<int, 9> count{};
        std::array<int, 9> posRow{};
        for (int row = 0; row < 9; ++row) {
            if (state.board[row * 9 + col] != 0) {
                continue;
            }
            const uint16_t mask = state.candidates(row, col);
            for (int d = 1; d <= 9; ++d) {
                if (mask & DigitBit(d)) {
                    ++count[d - 1];
                    posRow[d - 1] = row;
                }
            }
        }
        for (int d = 1; d <= 9; ++d) {
            if (count[d - 1] == 1 && state.place(posRow[d - 1], col, d)) {
                ++techniques.hiddenSingles;
                progress = true;
            }
        }
    }

    for (int box = 0; box < 9; ++box) {
        const int br = (box / 3) * 3;
        const int bc = (box % 3) * 3;
        std::array<int, 9> count{};
        std::array<int, 9> posRow{};
        std::array<int, 9> posCol{};

        for (int dr = 0; dr < 3; ++dr) {
            for (int dc = 0; dc < 3; ++dc) {
                const int row = br + dr;
                const int col = bc + dc;
                if (state.board[row * 9 + col] != 0) {
                    continue;
                }
                const uint16_t mask = state.candidates(row, col);
                for (int d = 1; d <= 9; ++d) {
                    if (mask & DigitBit(d)) {
                        ++count[d - 1];
                        posRow[d - 1] = row;
                        posCol[d - 1] = col;
                    }
                }
            }
        }

        for (int d = 1; d <= 9; ++d) {
            if (count[d - 1] == 1 && state.place(posRow[d - 1], posCol[d - 1], d)) {
                ++techniques.hiddenSingles;
                progress = true;
            }
        }
    }

    return progress;
}

bool ApplyNakedPairs(BoardState &state, AssignmentTechniqueStats &techniques) {
    auto ScanUnit = [&](const std::vector<int> &unit) -> bool {
        for (int i = 0; i < static_cast<int>(unit.size()); ++i) {
            const int idx1 = unit[i];
            const uint16_t m1 = state.candidates(idx1 / 9, idx1 % 9);
            if (Popcount(m1) != 2) {
                continue;
            }
            for (int j = i + 1; j < static_cast<int>(unit.size()); ++j) {
                const int idx2 = unit[j];
                const uint16_t m2 = state.candidates(idx2 / 9, idx2 % 9);
                if (m1 != m2 || Popcount(m2) != 2) {
                    continue;
                }

                for (int k = 0; k < static_cast<int>(unit.size()); ++k) {
                    if (k == i || k == j) {
                        continue;
                    }
                    const int idx = unit[k];
                    const int row = idx / 9;
                    const int col = idx % 9;
                    if (state.board[idx] != 0) {
                        continue;
                    }

                    const uint16_t mask = state.candidates(row, col);
                    if ((mask & m1) == 0) {
                        continue;
                    }

                    const uint16_t remaining = static_cast<uint16_t>(mask & ~m1);
                    if (Popcount(remaining) == 1) {
                        const int value = BitToDigit(remaining);
                        if (state.place(row, col, value)) {
                            ++techniques.nakedPairs;
                            return true;
                        }
                    }
                }
            }
        }
        return false;
    };

    return ApplyToUnits(state, ScanUnit);
}

bool ApplyHiddenPairs(BoardState &state, AssignmentTechniqueStats &techniques) {
    auto ScanUnit = [&](const std::vector<int> &unit) -> bool {
        std::array<uint16_t, 9> appears{};
        for (int d = 1; d <= 9; ++d) {
            uint16_t occ = 0;
            for (int i = 0; i < static_cast<int>(unit.size()); ++i) {
                const int idx = unit[i];
                const int row = idx / 9;
                const int col = idx % 9;
                if (state.candidates(row, col) & DigitBit(d)) {
                    occ = static_cast<uint16_t>(occ | static_cast<uint16_t>(1u << i));
                }
            }
            appears[d - 1] = occ;
        }

        for (int d1 = 1; d1 <= 9; ++d1) {
            if (Popcount(appears[d1 - 1]) != 2) {
                continue;
            }
            for (int d2 = d1 + 1; d2 <= 9; ++d2) {
                if (appears[d1 - 1] != appears[d2 - 1] || Popcount(appears[d2 - 1]) != 2) {
                    continue;
                }

                const uint16_t keep = static_cast<uint16_t>(DigitBit(d1) | DigitBit(d2));
                for (int i = 0; i < static_cast<int>(unit.size()); ++i) {
                    if (!(appears[d1 - 1] & static_cast<uint16_t>(1u << i))) {
                        continue;
                    }

                    const int idx = unit[i];
                    const int row = idx / 9;
                    const int col = idx % 9;
                    if (state.board[idx] != 0) {
                        continue;
                    }

                    const uint16_t mask = state.candidates(row, col);
                    const uint16_t remaining = static_cast<uint16_t>(mask & keep);
                    if (Popcount(remaining) == 1) {
                        const int value = BitToDigit(remaining);
                        if (state.place(row, col, value)) {
                            ++techniques.hiddenPairs;
                            return true;
                        }
                    }
                }
            }
        }

        return false;
    };

    return ApplyToUnits(state, ScanUnit);
}

}  // namespace

void RunHumanTechniques(BoardState &state, AssignmentTechniqueStats &techniques) {
    // Run easier rules first; restart from top after each successful change.
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

}  // namespace assignment_internal
