#include "sudoku.h"
#include "utility.h"
#include <chrono>
#include <filesystem>
#include <iostream>

using namespace sudoku;

namespace {

int CheckResult(const SolveResult &res) {
  if (!res.validInput)
    return (std::cerr << "Error: Invalid input\n", 1);
  if (!res.solved || res.solutionCount == 0)
    return (std::cerr << "Error: No solution\n", 1);
  if (res.solutionCount > 1)
    return (std::cerr << "Error: Not unique\n", 1);
  return 0;
}

int HandleGenerate(int argc, char **argv) {
  if (argc < 3)
    return (PrintUsage(std::cout), 1);
  auto target = ParseDifficulty(argv[2]);
  if (!target)
    return (std::cerr << "Error: Unknown difficulty " << argv[2] << "\n", 1);

  bool sym = (argc >= 4 && std::string(argv[3]) == "--symmetry");
  auto gen = GenerateSudoku(*target, sym);
  if (!gen)
    return (std::cerr << "Error: Generation failed\n", 1);

  std::filesystem::create_directories("generated_puzzle");
  std::string path = BuildGeneratedPuzzlePath(*target), err;
  if (!SaveSudokuToTextFile(path, gen->puzzle, err))
    return (std::cerr << "Error: " << err << "\n", 1);

  PrintGenerateReport(path, *target, gen->detectedDifficulty, std::cout);
  return 0;
}

} // namespace

int main(int argc, char **argv) {
  if (argc < 2)
    return (PrintUsage(std::cout), 1);
  std::string cmd = argv[1];
  if (cmd == "generate")
    return HandleGenerate(argc, argv);

  Board board{};
  std::string err;
  if (argc < 3)
    return (PrintUsage(std::cout), 1);
  if (!LoadSudokuFromTextFile(argv[2], board, err))
    return (std::cerr << "Error: " << err << "\n", 1);

  if (cmd == "print")
    return (std::cout << SudokuToPrettyString(board) << "\n", 0);

  if (cmd == "solve") {
    auto start = std::chrono::steady_clock::now();
    auto res = SolveSudokuUnique(board);
    auto ms = std::chrono::duration<double, std::milli>(
                  std::chrono::steady_clock::now() - start)
                  .count();
    if (int v = CheckResult(res))
      return v;
    return (PrintSolveReport(res, ms, std::cout), 0);
  }

  if (cmd == "grade") {
    auto metrics = SolveSudokuUnique(board, 2, true);
    if (int v = CheckResult(metrics))
      return v;
    auto diff = GradeSudoku(metrics);
    if (!diff)
      return (std::cerr << "Error: Grading failed\n", 1);
    return (PrintGradeReport(*diff, metrics, std::cout), 0);
  }

  return (std::cerr << "Error: Unknown command " << cmd << "\n",
          PrintUsage(std::cout), 1);
}
