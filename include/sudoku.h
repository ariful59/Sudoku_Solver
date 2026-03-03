#ifndef SUDOKU_H
#define SUDOKU_H

#include <array>
#include <cstdint>
#include <optional>
#include <string>

namespace sudoku {

  struct Board {
    std::array<int, 81> cells{};

    int& at(int row, int col) { return cells[row * 9 + col]; }
    int at(int row, int col) const { return cells[row * 9 + col]; }
  };

  enum class Difficulty { EASY = 0, MEDIUM = 1, HARD = 2, SAMURAI = 3 };

  struct SolveStats {
    int branches = 0;
    int backtracks = 0;
    int maxDepth = 0;
  };

  struct TechniqueStats {
    int nakedSingles = 0;
    int hiddenSingles = 0;
    int nakedPairs = 0;
    int hiddenPairs = 0;
    bool usedBacktracking = false;
  };

  struct SolveResult {
    bool validInput = false;
    bool solved = false;
    int solutionCount = 0;
    Board solution{};
    SolveStats stats{};
    TechniqueStats techniques{};
  };

  struct GenerationResult {
    Board sudoku{};
    Difficulty detectedDifficulty = Difficulty::EASY;
  };

  bool LoadSudokuFromTextFile(const std::string& path, Board& board,
    std::string& error);
  bool SaveSudokuToTextFile(const std::string& path, const Board& board,
    std::string& error);
  std::string SudokuToPrettyString(const Board& board);
  SolveResult SolveSudoku(const Board& input, bool requireUnique = true,
    bool useHumanTechniques = false);
  bool GenerateSolvedBoard(Board& outBoard);
  std::optional<GenerationResult> GenerateSudoku(Difficulty targetDifficulty,
    bool symmetry180 = false,
    int attempts = 100);
  std::optional<Difficulty> GradeSudoku(const Board& board, SolveResult& metrics);
  std::optional<Difficulty> GradePuzzleDifficulty(const SolveResult& metrics);

} // namespace sudoku

#endif
