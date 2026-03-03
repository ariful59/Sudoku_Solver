#ifndef SUDOKU_H
#define SUDOKU_H

#include <array>
#include <cstdint>
#include <optional>
#include <string>

namespace sudoku {

struct Board {
  std::array<int, 81> cells{};

  int &at(int row, int col) { return cells[row * 9 + col]; }
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

struct GeneratedPuzzle {
  Board puzzle{};
  Board solution{};
  Difficulty detectedDifficulty = Difficulty::EASY;
};

bool LoadSudokuFromTextFile(const std::string &path, Board &board,
                            std::string &error);
bool SaveSudokuToTextFile(const std::string &path, const Board &board,
                          std::string &error);
std::string SudokuToPrettyString(const Board &board);
bool IsBoardConsistent(const Board &board);
SolveResult SolveSudokuUnique(const Board &input, int solutionLimit = 2,
                              bool useHumanTechniques = false);
bool GenerateSolvedBoard(Board &outBoard, bool useHumanTechniques = false);
std::optional<GeneratedPuzzle> GenerateSudoku(Difficulty targetDifficulty,
                                              bool symmetry180 = false,
                                              int attempts = 60);
std::optional<Difficulty> GradeSudoku(const SolveResult &metrics);

} // namespace sudoku

#endif
