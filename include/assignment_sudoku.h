#ifndef ASSIGNMENT_SUDOKU_H
#define ASSIGNMENT_SUDOKU_H

#include <array>
#include <cstdint>
#include <string>

struct AssignmentSudoku
{
    std::array<int, 81> cells{};

    int &at(int row, int col) { return cells[row * 9 + col]; }
    int at(int row, int col) const { return cells[row * 9 + col]; }
};

struct AssignmentSolveStats
{
    int branches = 0;
    int backtracks = 0;
    int maxDepth = 0;
};

struct AssignmentTechniqueStats
{
    bool usedBacktracking = false;
};

struct AssignmentSolveResult
{
    bool validInput = false;
    bool solved = false;
    int solutionCount = 0;
    AssignmentSudoku solution{};
    AssignmentSolveStats stats{};
    AssignmentTechniqueStats techniques{};
};

bool LoadSudokuFromTextFile(const std::string &path, AssignmentSudoku &board,
                            std::string &error);
bool SaveSudokuToTextFile(const std::string &path,
                          const AssignmentSudoku &board, std::string &error);
std::string SudokuToPrettyString(const AssignmentSudoku &board);
bool IsBoardConsistent(const AssignmentSudoku &board);
AssignmentSolveResult SolveSudokuUnique(const AssignmentSudoku &input);

#endif
