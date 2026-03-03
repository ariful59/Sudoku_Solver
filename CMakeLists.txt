cmake_minimum_required(VERSION 3.16)

project(QlikSudokuAssignment LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(sudoku
  src/main.cpp
  src/utility.cpp
  src/puzzle_io.cpp
  src/sudoku_rules.cpp
  src/board_state.cpp
  src/human_logic.cpp
  src/solver_core.cpp
  src/generator.cpp
  src/grader.cpp
)

target_include_directories(sudoku
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if (MSVC)
    target_compile_options(sudoku PRIVATE /W4 /permissive-)
else()
    target_compile_options(sudoku PRIVATE -Wall -Wextra -Wpedantic)
endif()
