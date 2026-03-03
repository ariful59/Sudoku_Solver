#ifndef UTILITY_H
#define UTILITY_H

#include <iosfwd>
#include <optional>
#include <string>

#include "assignment_sudoku.h"

std::optional<AssignmentDifficulty> ParseDifficulty(const std::string &text);
const char *DifficultyToString(AssignmentDifficulty difficulty);
std::string BuildGeneratedPuzzlePath(AssignmentDifficulty difficulty);
void PrintUsage(std::ostream &out);
void PrintSolveReport(const AssignmentSolveResult &result, double elapsedMs, std::ostream &out);
void PrintGradeReport(AssignmentDifficulty difficulty, const AssignmentSolveResult &metrics,
                      std::ostream &out);
void PrintGenerateReport(const std::string &outputPath, AssignmentDifficulty target,
                         AssignmentDifficulty detected, std::ostream &out);

#endif
