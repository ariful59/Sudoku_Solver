#include "sudoku.h"

#include <algorithm>
#include <array>
#include <limits>
#include <numeric>
#include <random>
#include <vector>

namespace sudoku {

namespace {
int DifficultyDistance(Difficulty lhs, Difficulty rhs) {
  return std::abs(static_cast<int>(lhs) - static_cast<int>(rhs));
}

int TargetEmptyCells(Difficulty difficulty) {
  switch (difficulty) {
  case Difficulty::EASY:
    return 38;
  case Difficulty::MEDIUM:
    return 46;
  case Difficulty::HARD:
    return 53;
  case Difficulty::SAMURAI:
    return 58;
  default:
    return 46;
  }
}

void ApplyDigitPermutation(Board &board, std::mt19937 &rng) {
  std::array<int, 10> map{};
  std::array<int, 9> digits{};
  std::iota(digits.begin(), digits.end(), 1);
  std::shuffle(digits.begin(), digits.end(), rng);
  for (int i = 1; i <= 9; ++i)
    map[i] = digits[i - 1];
  for (int &v : board.cells)
    if (v != 0)
      v = map[v];
}

void RandomizeSolvedBoard(Board &board, std::mt19937 &rng) {
  auto SwapRows = [&](int r1, int r2) {
    for (int c = 0; c < 9; ++c)
      std::swap(board.at(r1, c), board.at(r2, c));
  };
  auto SwapCols = [&](int c1, int c2) {
    for (int r = 0; r < 9; ++r)
      std::swap(board.at(r, c1), board.at(r, c2));
  };

  ApplyDigitPermutation(board, rng);
  for (int b = 0; b < 3; ++b) {
    std::array<int, 3> r{b * 3, b * 3 + 1, b * 3 + 2};
    std::shuffle(r.begin(), r.end(), rng);
    SwapRows(b * 3, r[0]);
    SwapRows(b * 3 + 1, r[1]);
    SwapRows(b * 3, r[0]);
    SwapRows(b * 3 + 1, r[1]);
    SwapRows(b * 3 + 2, r[2]);
  }
  for (int s = 0; s < 3; ++s) {
    std::array<int, 3> colIdx{s * 3, s * 3 + 1, s * 3 + 2};
    std::shuffle(colIdx.begin(), colIdx.end(), rng);
    SwapCols(s * 3, colIdx[0]);
    SwapCols(s * 3 + 1, colIdx[1]);
    SwapCols(s * 3 + 2, colIdx[2]);
  }
}

int CountEmptyCells(const Board &board) {
  int empty = 0;
  for (int v : board.cells)
    if (v == 0)
      ++empty;
  return empty;
}
} // namespace

std::optional<GeneratedPuzzle> GenerateSudoku(Difficulty targetDifficulty,
                                              bool symmetry180, int attempts) {
  std::mt19937 rng(std::random_device{}());
  bool foundAny = false;
  GeneratedPuzzle best{};
  int bestDist = std::numeric_limits<int>::max(), bestEmpty = -1;
  const int targetEmpty = TargetEmptyCells(targetDifficulty);

  for (int attempt = 0; attempt < std::max(1, attempts); ++attempt) {
    Board solution{};
    if (!GenerateSolvedBoard(solution, false))
      continue;
    RandomizeSolvedBoard(solution, rng);

    Board puzzle = solution;
    std::vector<int> order;
    for (int i = 0; i < 81; ++i)
      if (!symmetry180 || i <= (80 - i))
        order.push_back(i);
    std::shuffle(order.begin(), order.end(), rng);

    for (int idx : order) {
      if (puzzle.cells[idx] == 0)
        continue;
      const int s1 = puzzle.cells[idx], mirror = 80 - idx,
                s2 = puzzle.cells[mirror];
      puzzle.cells[idx] = 0;
      if (symmetry180)
        puzzle.cells[mirror] = 0;

      const SolveResult check = SolveSudokuUnique(puzzle, 2, false);
      if (!check.validInput || check.solutionCount != 1) {
        puzzle.cells[idx] = s1;
        if (symmetry180)
          puzzle.cells[mirror] = s2;
        continue;
      }
      if (CountEmptyCells(puzzle) >= targetEmpty)
        break;
    }

    const SolveResult metrics = SolveSudokuUnique(puzzle, 2, true);
    const std::optional<Difficulty> detected = GradeSudoku(metrics);
    if (!detected)
      continue;

    const int dist = DifficultyDistance(*detected, targetDifficulty);
    const int empty = CountEmptyCells(puzzle);
    if (!foundAny || dist < bestDist ||
        (dist == bestDist && empty > bestEmpty)) {
      foundAny = true;
      best.puzzle = puzzle;
      best.solution = solution;
      best.detectedDifficulty = *detected;
      bestDist = dist;
      bestEmpty = empty;
    }
    if (dist == 0)
      break;
  }
  return foundAny ? std::make_optional(best) : std::nullopt;
}

} // namespace sudoku
