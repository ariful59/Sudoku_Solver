#include "sudoku.h"
#include "utility.h"
#include <chrono>
#include <filesystem>
#include <iostream>

using namespace sudoku;

namespace {

  int CheckResult(const SolveResult& res) {
    if (!res.validInput)
      return (std::cerr << "Error: Invalid input\n", 1);
    if (!res.solved || res.solutionCount == 0)
      return (std::cerr << "Error: No solution\n", 1);
    if (res.solutionCount > 1)
      return (std::cerr << "Error: Not unique\n", 1);
    return 0;
  }

  int HandleGenerate(int argc, char** argv) {
    auto start = std::chrono::steady_clock::now();
    auto target = ParseDifficulty(argv[2]);
    if (!target) {
      std::cerr << "Error: Unknown difficulty " << argv[2] << "\n";
      return (PrintUsage(std::cout), 1);
    }
    bool sym = false;
    if (argc >= 4) {
      if (std::string(argv[3]) != "--symmetry")
        return (PrintUsage(std::cout), 1);
      sym = true;
    }
    auto gen = GenerateSudoku(*target, sym);
    if (!gen)
      return (std::cerr << "Error: Could not able to generated sudoku with requested difficulty\n", 1);

    std::filesystem::create_directories("generated_sudokus");
    std::string path = BuildGeneratedPuzzlePath(*target), err;
    if (!SaveSudokuToTextFile(path, gen->sudoku, err))
      return (std::cerr << "Error: " << err << "\n", 1);

    auto ms = std::chrono::duration<double, std::milli>(
      std::chrono::steady_clock::now() - start)
      .count();
    PrintGenerateReport(path, *target, gen->detectedDifficulty, ms, std::cout);
    std::cout << SudokuToPrettyString(gen->sudoku) << '\n';
    return 0;
  }

} // namespace

int main(int argc, char** argv) {
  if (argc < 3)
    return (PrintUsage(std::cout), 1);
  std::string cmd = argv[1];
  if (cmd == "generate")
    return HandleGenerate(argc, argv);

  Board board{};
  std::string err;
  if (!LoadSudokuFromTextFile(argv[2], board, err))
    return (std::cerr << "Error: " << err << "\n", 1);

  if (cmd == "print") {
    std::cout << SudokuToPrettyString(board) << '\n';
  }
  else if (cmd == "solve") {
    auto start = std::chrono::steady_clock::now();
    const bool requireUnique = true;
    const bool useHumanTechniques = false;
    auto res = SolveSudoku(board, requireUnique, useHumanTechniques);
    auto ms = std::chrono::duration<double, std::milli>(
      std::chrono::steady_clock::now() - start)
      .count();
    if (int v = CheckResult(res))
      return v;
    PrintSolveReport(res, ms, std::cout);
  }
  else if (cmd == "grade") {
    auto start = std::chrono::steady_clock::now();
    SolveResult metrics{};
    auto diff = GradeSudoku(board, metrics);
    if (!diff)
      return (std::cerr << "Error: Grading failed\n", 1);
    auto ms = std::chrono::duration<double, std::milli>(
      std::chrono::steady_clock::now() - start)
      .count();
    PrintGradeReport(*diff, metrics, ms, std::cout);
  }
  else {
    PrintUsage(std::cout);
    return 1;
  }
  return 0;
}
