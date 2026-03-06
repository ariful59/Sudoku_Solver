#include "sudoku.h"

#include <algorithm>
#include <array>
#include <limits>
#include <numeric>
#include <random>
#include <vector>

namespace sudoku
{
  namespace
  {
    int DifficultyDistance(Difficulty lhs, Difficulty rhs)
    {
      return std::abs(static_cast<int>(lhs) - static_cast<int>(rhs));
    }

    void BuildDigitPermutation(std::array<int, 10>& map, std::mt19937& rng)
    {
      map.fill(0);
      std::array<int, 9> digits{};
      std::iota(digits.begin(), digits.end(), 1);
      std::shuffle(digits.begin(), digits.end(), rng);
      for (int i = 1; i <= 9; ++i)
        map[i] = digits[i - 1];
    }

    void BuildRowOrColPermutation(std::array<int, 9>& perm, std::mt19937& rng)
    {
      for (int group = 0; group < 3; ++group)
      {
        std::array<int, 3> idx{ group * 3, group * 3 + 1, group * 3 + 2 };
        std::shuffle(idx.begin(), idx.end(), rng);
        perm[group * 3 + 0] = idx[0];
        perm[group * 3 + 1] = idx[1];
        perm[group * 3 + 2] = idx[2];
      }
    }

    void RandomizeSolvedBoard(Board& board, std::mt19937& rng)
    {
      std::array<int, 10> digitPerm{};
      std::array<int, 9> rowPerm{};
      std::array<int, 9> colPerm{};
      BuildDigitPermutation(digitPerm, rng);
      BuildRowOrColPermutation(rowPerm, rng);
      BuildRowOrColPermutation(colPerm, rng);

      Board remapped{};
      for (int row = 0; row < 9; ++row)
      {
        for (int col = 0; col < 9; ++col)
        {
          const int src = board.at(rowPerm[row], colPerm[col]);
          remapped.at(row, col) = (src == 0) ? 0 : digitPerm[src];
        }
      }
      board = remapped;
    }

  } // namespace

  std::optional<GenerationResult> GenerateSudoku(Difficulty targetDifficulty,
    bool symmetry180, int attempts)
  {
    std::mt19937 rng(std::random_device{}());
    bool foundCandidate = false;
    bool useHumanTechniques = false;
    const bool requireUnique = true;
    constexpr int maxEmptyCellsForGrading = 40;
    GenerationResult selected{};
    int closestDistance = std::numeric_limits<int>::max();

    for (int attempt = 0; attempt < attempts && closestDistance > 0; ++attempt)
    {
      Board solution{};
      if (!GenerateSolvedBoard(solution))
        continue;
      RandomizeSolvedBoard(solution, rng);

      Board sudoku = solution;
      std::vector<int> order;
      for (int i = 0; i < 81; ++i)
        if (!symmetry180 || i <= (80 - i))
          order.push_back(i);
      std::shuffle(order.begin(), order.end(), rng);

      int emptyCells = 0;
      for (int idx : order)
      {
        if (sudoku.cells[idx] == 0)
          continue;
        const int mirror = 80 - idx;
        const bool pairedRemoval = symmetry180 && mirror != idx;
        const int savedValue = sudoku.cells[idx];
        const int savedMirrorValue = sudoku.cells[mirror];
        sudoku.cells[idx] = 0;
        emptyCells++;
        if (pairedRemoval)
        {
          emptyCells++;
          sudoku.cells[mirror] = 0;
        }
        useHumanTechniques = (emptyCells > maxEmptyCellsForGrading);
        //uniqueness only after 40 use human techniques for grading purposes
        const SolveResult metrics =
          SolveSudoku(sudoku, requireUnique, useHumanTechniques);
        if (!metrics.validInput || metrics.solutionCount != 1)
        {
          sudoku.cells[idx] = savedValue;
          emptyCells--;
          if (pairedRemoval)
          {
            emptyCells--;
            sudoku.cells[mirror] = savedMirrorValue;
          }
          continue;
        }

        //grading starts when the sudoku as at 40 empty cells
        if (emptyCells <= maxEmptyCellsForGrading)
          continue;


        const std::optional<Difficulty> detected = GradePuzzleDifficulty(metrics);
        if (!detected)
          continue;
        const int dist = DifficultyDistance(*detected, targetDifficulty);
        if (dist < closestDistance || dist == 0) {
          foundCandidate = true;
          closestDistance = dist;
          selected.sudoku = sudoku;
          selected.detectedDifficulty = *detected;
        }
        if (closestDistance == 0) {
          break;
        }
      }
    }
    if (foundCandidate && closestDistance == 0) {
      return std::make_optional(selected);
    }
    return foundCandidate ? std::make_optional(selected) : std::nullopt;
  }

} // namespace sudoku
