#include "assignment_sudoku.h"

#include <fstream>
#include <sstream>
#include <vector>

bool LoadSudokuFromTextFile(const std::string &path, AssignmentSudoku &board,
                            std::string &error)
{
  std::ifstream input(path);
  if (!input.is_open())
  {
    error = "Could not open file: " + path;
    return false;
  }

  std::vector<std::string> lines;
  std::string line;
  while (std::getline(input, line))
  {
    if (!line.empty() && line.back() == '\r')
    {
      line.pop_back();
    }
    lines.push_back(line);
  }

  if (lines.size() < 9)
  {
    error =
        "Expected at least 9 lines, got " + std::to_string(lines.size()) + ".";
    return false;
  }

  for (int row = 0; row < 9; ++row)
  {
    int col = 0;
    for (char c : lines[row])
    {
      if (col >= 9)
      {
        break;
      }
      if (c >= '1' && c <= '9')
      {
        board.at(row, col++) = c - '0';
        continue;
      }
      if (c == '.' || c == '0' || c == '_' || c == '*')
      {
        board.at(row, col++) = 0;
        continue;
      }
      // Ignore separators/spaces to support pretty-formatted input.
      if (c == ' ' || c == '\t' || c == '|' || c == '+' || c == '-')
      {
        continue;
      }
      // Any other character is treated as an empty cell marker.
      board.at(row, col++) = 0;
    }
    if (col != 9)
    {
      error = "Line " + std::to_string(row + 1) + " does not contain 9 cells.";
      return false;
    }
  }

  if (!IsBoardConsistent(board))
  {
    error = "Input board violates Sudoku rules.";
    return false;
  }

  return true;
}

bool SaveSudokuToTextFile(const std::string &path,
                          const AssignmentSudoku &board, std::string &error)
{
  std::ofstream output(path);
  if (!output.is_open())
  {
    error = "Could not write file: " + path;
    return false;
  }
  for (int row = 0; row < 9; ++row)
  {
    for (int col = 0; col < 9; ++col)
    {
      const int value = board.at(row, col);
      output << (value == 0 ? '.' : static_cast<char>('0' + value));
    }
    if (row != 8)
    {
      output << '\n';
    }
  }
  return true;
}

std::string SudokuToPrettyString(const AssignmentSudoku &board)
{
  std::ostringstream oss;
  const char *sep = "+-------+-------+-------+";
  for (int row = 0; row < 9; ++row)
  {
    if (row % 3 == 0)
    {
      oss << sep << '\n';
    }
    oss << "| ";
    for (int col = 0; col < 9; ++col)
    {
      const int value = board.at(row, col);
      oss << (value == 0 ? '.' : static_cast<char>('0' + value)) << ' ';
      if ((col + 1) % 3 == 0)
      {
        oss << "| ";
      }
    }
    oss << '\n';
  }
  oss << sep;
  return oss.str();
}
