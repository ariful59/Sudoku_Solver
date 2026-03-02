#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <random>
#include <string>

#include "assignment_sudoku.h"

static void PrintUsage() {
  std::cout << "Qlik Sudoku Assignment\n\n";
  std::cout << "Usage:\n";
  std::cout << "  sudoku <file>\n";
  std::cout << "  sudoku <command> <file>\n";
}

int main(int argc, char **argv) {
  if (argc < 2) {
    PrintUsage();
    return 1;
  }

  const bool hasCommand = (argc >= 3);
  const std::string command = hasCommand ? argv[1] : "load";
  (void)command;
  const std::string inputPath = hasCommand ? argv[2] : argv[1];

  AssignmentSudoku board{};
  std::string error;
  if (!LoadSudokuFromTextFile(inputPath, board, error)) {
    std::cerr << "Error: " << error << '\n';
    return 1;
  }

  std::cout << SudokuToPrettyString(board) << '\n';
  return 0;
}
