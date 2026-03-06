#include <cassert>
#include <iostream>
#include <string>

#include "sudoku.h"

namespace {

sudoku::Board LoadBoardFromFile(const std::string& filename) {
  sudoku::Board board{};
  std::string error;
  const std::string path =
      std::string(TEST_SOURCE_DIR) + "/assignment_examples/" + filename;
  const bool loaded = sudoku::LoadSudokuFromTextFile(path, board, error);
  assert(loaded);
  assert(error.empty());
  return board;
}

void TestSolveUniqueEasy() {
  const sudoku::SolveResult result =
      sudoku::SolveSudoku(LoadBoardFromFile("easy.txt"),
                          /*requireUnique=*/true,
                          /*useHumanTechniques=*/false);
  assert(result.validInput);
  assert(result.solved);
  assert(result.solutionCount == 1);
}

void TestSolveUniqueHardUsesDfs() {
  const sudoku::SolveResult result =
      sudoku::SolveSudoku(LoadBoardFromFile("hard.txt"),
                          /*requireUnique=*/true,
                          /*useHumanTechniques=*/false);
  assert(result.validInput);
  assert(result.solved);
  assert(result.solutionCount == 1);
  assert(result.stats.branches > 0);
  assert(result.stats.backtracks > 0);
  assert(result.techniques.usedBacktracking);
}

void TestDetectMultipleSolutions() {
  const sudoku::SolveResult result =
      sudoku::SolveSudoku(LoadBoardFromFile("multiple.txt"),
                          /*requireUnique=*/true,
                          /*useHumanTechniques=*/false);
  assert(result.validInput);
  assert(result.solved);
  assert(result.solutionCount > 1);
}

void TestRejectInvalidInput() {
  const sudoku::SolveResult result =
      sudoku::SolveSudoku(LoadBoardFromFile("invalid.txt"),
                          /*requireUnique=*/true,
                          /*useHumanTechniques=*/false);
  assert(!result.validInput);
  assert(!result.solved);
  assert(result.solutionCount == 0);
}

void TestUnsatValidPuzzle() {
  const sudoku::SolveResult result =
      sudoku::SolveSudoku(LoadBoardFromFile("unsat.txt"),
                          /*requireUnique=*/true,
                          /*useHumanTechniques=*/false);
  assert(result.validInput);
  assert(!result.solved);
  assert(result.solutionCount == 0);
}

} // namespace

int main() {
  TestSolveUniqueEasy();
  TestSolveUniqueHardUsesDfs();
  TestDetectMultipleSolutions();
  TestRejectInvalidInput();
  TestUnsatValidPuzzle();
  std::cout << "Solver suite passed (5 tests).\n";
  return 0;
}
