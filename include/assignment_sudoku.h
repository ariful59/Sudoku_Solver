#ifndef ASSIGNMENT_SUDOKU_H
#define ASSIGNMENT_SUDOKU_H

#include <array>
#include <cstdint>
#include <optional>
#include <string>

struct AssignmentSudoku {
    std::array<int, 81> cells{};

    int &at(int row, int col) { return cells[row * 9 + col]; }
    int at(int row, int col) const { return cells[row * 9 + col]; }
};

bool LoadSudokuFromTextFile(const std::string &path, AssignmentSudoku &board,
                            std::string &error);
bool SaveSudokuToTextFile(const std::string &path,
                          const AssignmentSudoku &board, std::string &error);
std::string SudokuToString(const AssignmentSudoku &board);
std::string SudokuToPrettyString(const AssignmentSudoku &board);
bool IsBoardConsistent(const AssignmentSudoku &board);

#endif
