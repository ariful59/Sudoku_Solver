cmake_minimum_required(VERSION 3.16)

project(SudokuSolver LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(sudoku
  src/main.cpp
  src/utility.cpp
  src/sudoku_io.cpp
  src/board_state.cpp
  src/human_logic.cpp
  src/solver_engine.cpp
  src/solver.cpp
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

enable_testing()

add_executable(test_solver_suite
  unit_test/test_solver_suite.cpp
  src/sudoku_io.cpp
  src/board_state.cpp
  src/human_logic.cpp
  src/solver_engine.cpp
  src/solver.cpp
)

target_include_directories(test_solver_suite
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_compile_definitions(test_solver_suite
    PRIVATE
        TEST_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)

if (MSVC)
    target_compile_options(test_solver_suite PRIVATE /W4 /permissive-)
else()
    target_compile_options(test_solver_suite PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_test(NAME solver_suite COMMAND test_solver_suite)
