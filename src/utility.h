#ifndef UTILITY_H
#define UTILITY_H

#include <iosfwd>
#include <optional>
#include <string>

#include "sudoku.h"

namespace sudoku {

std::optional<Difficulty> ParseDifficulty(const std::string &text);
std::string DifficultyToString(Difficulty difficulty);
std::string BuildGeneratedPuzzlePath(Difficulty difficulty);
void PrintUsage(std::ostream &out);
void PrintSolveReport(const SolveResult &result, double elapsedMs,
                      std::ostream &out);
void PrintGradeReport(Difficulty difficulty, const SolveResult &metrics,
                      std::ostream &out);
void PrintGenerateReport(const std::string &outputPath, Difficulty target,
                         Difficulty detected, std::ostream &out);

} // namespace sudoku

#endif
