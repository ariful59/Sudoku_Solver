#include "assignment_sudoku.h"

#include <algorithm>
#include <array>
#include <limits>
#include <numeric>
#include <random>
#include <vector>

namespace {
int DifficultyDistance(AssignmentDifficulty lhs, AssignmentDifficulty rhs) {
    return std::abs(static_cast<int>(lhs) - static_cast<int>(rhs));
}

int TargetEmptyCells(AssignmentDifficulty difficulty) {
    switch (difficulty) {
        case AssignmentDifficulty::EASY:
            return 38;
        case AssignmentDifficulty::MEDIUM:
            return 46;
        case AssignmentDifficulty::HARD:
            return 53;
        case AssignmentDifficulty::SAMURAI:
            return 58;
        default:
            return 46;
    }
}

void ApplyDigitPermutation(AssignmentSudoku &board, std::mt19937 &rng) {
    std::array<int, 10> map{};
    std::array<int, 9> digits{};
    std::iota(digits.begin(), digits.end(), 1);
    std::shuffle(digits.begin(), digits.end(), rng);
    for (int i = 1; i <= 9; ++i) {
        map[i] = digits[i - 1];
    }
    for (int &v : board.cells) {
        if (v != 0) {
            v = map[v];
        }
    }
}

void SwapRows(AssignmentSudoku &board, int r1, int r2) {
    for (int c = 0; c < 9; ++c) {
        std::swap(board.cells[r1 * 9 + c], board.cells[r2 * 9 + c]);
    }
}

void SwapCols(AssignmentSudoku &board, int c1, int c2) {
    for (int r = 0; r < 9; ++r) {
        std::swap(board.cells[r * 9 + c1], board.cells[r * 9 + c2]);
    }
}

void RandomizeSolvedBoard(AssignmentSudoku &board, std::mt19937 &rng) {
    ApplyDigitPermutation(board, rng);

    for (int band = 0; band < 3; ++band) {
        std::array<int, 3> rows{band * 3, band * 3 + 1, band * 3 + 2};
        std::shuffle(rows.begin(), rows.end(), rng);
        SwapRows(board, band * 3, rows[0]);
        SwapRows(board, band * 3 + 1, rows[1]);
        SwapRows(board, band * 3 + 2, rows[2]);
    }

    for (int stack = 0; stack < 3; ++stack) {
        std::array<int, 3> cols{stack * 3, stack * 3 + 1, stack * 3 + 2};
        std::shuffle(cols.begin(), cols.end(), rng);
        SwapCols(board, stack * 3, cols[0]);
        SwapCols(board, stack * 3 + 1, cols[1]);
        SwapCols(board, stack * 3 + 2, cols[2]);
    }
}

int CountEmptyCells(const AssignmentSudoku &board) {
    int empty = 0;
    for (int v : board.cells) {
        if (v == 0) {
            ++empty;
        }
    }
    return empty;
}
} // namespace

std::optional<AssignmentGeneratedPuzzle> GenerateSudoku(
    AssignmentDifficulty targetDifficulty, bool symmetry180, int attempts) {
    if (attempts <= 0) {
        attempts = 1;
    }

    std::mt19937 rng(std::random_device{}());
    bool foundAny = false;
    AssignmentGeneratedPuzzle best{};
    int bestDistance = std::numeric_limits<int>::max();
    int bestEmptyCells = -1;
    const int targetEmpty = TargetEmptyCells(targetDifficulty);

    for (int attempt = 0; attempt < attempts; ++attempt) {
        AssignmentSudoku solution{};
        if (!GenerateSolvedBoard(solution, false)) {
            continue;
        }
        RandomizeSolvedBoard(solution, rng);

        AssignmentSudoku puzzle = solution;
        std::vector<int> order;
        order.reserve(81);
        for (int i = 0; i < 81; ++i) {
            if (!symmetry180 || i <= (80 - i)) {
                order.push_back(i);
            }
        }
        std::shuffle(order.begin(), order.end(), rng);

        for (int idx : order) {
            if (puzzle.cells[idx] == 0) {
                continue;
            }
            const int saved = puzzle.cells[idx];
            const int mirror = 80 - idx;
            const int mirrorSaved = puzzle.cells[mirror];
            puzzle.cells[idx] = 0;
            if (symmetry180) {
                puzzle.cells[mirror] = 0;
            }

            const AssignmentSolveResult uniqueCheck = SolveSudokuUnique(puzzle, 2, false);
            if (!uniqueCheck.validInput || uniqueCheck.solutionCount != 1) {
                puzzle.cells[idx] = saved;
                if (symmetry180) {
                    puzzle.cells[mirror] = mirrorSaved;
                }
                continue;
            }
            if (CountEmptyCells(puzzle) >= targetEmpty) {
                break;
            }
        }

        const AssignmentSolveResult gradeMetrics = SolveSudokuUnique(puzzle, 2, true);
        const std::optional<AssignmentDifficulty> detected = GradeSudoku(gradeMetrics);
        if (!detected.has_value()) {
            continue;
        }

        const int distance = DifficultyDistance(*detected, targetDifficulty);
        const int emptyCells = CountEmptyCells(puzzle);
        if (!foundAny || distance < bestDistance ||
            (distance == bestDistance && emptyCells > bestEmptyCells)) {
            foundAny = true;
            best.puzzle = puzzle;
            best.solution = solution;
            best.detectedDifficulty = *detected;
            bestDistance = distance;
            bestEmptyCells = emptyCells;
        }
        if (distance == 0) {
            break;
        }
    }

    if (!foundAny) {
        return std::nullopt;
    }
    return best;
}
