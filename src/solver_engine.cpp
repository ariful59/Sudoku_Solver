#include "solver_engine.h"

#include <algorithm>
#include <limits>
#include <utility>
#include <vector>

#include "board_state.h"
#include "human_logic.h"

namespace sudoku::internal {
  namespace {

    struct DfsSolver {
      BoardState state;
      SolveStats stats{};
      TechniqueStats techniques{};
      std::array<int, 81> firstSolution{};
      int solutionCount = 0;
      int solutionLimit = 0;

      explicit DfsSolver(const Board& input, int limit) : state(input), solutionLimit(limit) {}

      bool PickMRVCell(int& outIdx, uint16_t& outMask) const {
        // Minimum Remaining Values - pick the empty cell with the fewest candidates
        int bestIdx = -1;
        int bestCount = std::numeric_limits<int>::max();
        uint16_t bestMask = 0;

        for (int idx = 0; idx < 81; ++idx) {
          if (state.board[idx] != 0)
            continue;

          const int row = idx / 9;
          const int col = idx % 9;
          const uint16_t mask = state.candidates(row, col);
          const int count = Popcount(mask);
          if (count == 0)
            return false;
          if (count < bestCount) {
            bestCount = count;
            bestIdx = idx;
            bestMask = mask;
            if (count == 1)
              break;
          }
        }

        outIdx = bestIdx;
        outMask = bestMask;
        return true;
      }

      bool PropagateNakedSingles(std::vector<std::pair<int, int>>& trail) {
        bool changed = true;
        while (changed) {
          changed = false;
          for (int idx = 0; idx < 81; ++idx) {
            if (state.board[idx] != 0)
              continue;

            const int row = idx / 9;
            const int col = idx % 9;
            const uint16_t mask = state.candidates(row, col);
            const int count = Popcount(mask);
            if (count == 0)
              return false;
            if (count == 1) {
              const int value = BitToDigit(mask);
              if (!state.place(row, col, value))
                return false;
              trail.emplace_back(idx, value);
              changed = true;
            }
          }
        }
        return true;
      }

      void Rollback(const std::vector<std::pair<int, int>>& trail) {
        for (size_t i = trail.size(); i-- > 0;) {
          const int idx = trail[i].first;
          const int value = trail[i].second;
          state.remove(idx / 9, idx % 9, value);
        }
      }

      void Search(int depth) {
        if (solutionCount >= solutionLimit)
          return;
        stats.maxDepth = std::max(stats.maxDepth, depth);

        std::vector<std::pair<int, int>> forced;
        if (!PropagateNakedSingles(forced)) {
          Rollback(forced);
          ++stats.backtracks;
          return;
        }

        int idx = -1;
        uint16_t mask = 0;
        if (!PickMRVCell(idx, mask)) {
          Rollback(forced);
          return;
        }

        if (idx == -1) {
          ++solutionCount;
          if (solutionCount == 1)
            firstSolution = state.board;
          Rollback(forced);
          return;
        }

        std::vector<int> digits;
        for (uint16_t bits = mask; bits != 0;
          //clear the lowest set bit in each iteration
          bits &= static_cast<uint16_t>(bits - 1)) {
          // extract the lowest set bit using & and two's complement
          const uint16_t lowestBit = static_cast<uint16_t>(
            bits & static_cast<uint16_t>(-static_cast<int16_t>(bits)));
          digits.push_back(BitToDigit(lowestBit));
        }

        if (digits.size() > 1) {
          ++stats.branches;
          techniques.usedBacktracking = true;
        }

        const int row = idx / 9;
        const int col = idx % 9;
        for (int value : digits) {
          if (!state.place(row, col, value))
            continue;
          Search(depth + 1);
          state.remove(row, col, value);
          if (solutionCount >= solutionLimit) {
            Rollback(forced);
            return;
          }
        }

        Rollback(forced);
        ++stats.backtracks;
      }
    };

  } // namespace

  SolveResult SolveWithConfig(const Board& input, bool requireUnique,
    bool useHumanTechniques) {
    SolveResult result{};
    // requireUnique = true means we want to check for uniqueness, which requires finding up to 2 solutions
    DfsSolver solver(input, requireUnique ? 2 : 1);
    result.validInput = solver.state.valid;
    if (!solver.state.valid)
      return result;

    if (useHumanTechniques) {
      RunHumanTechniques(solver.state, solver.techniques);
    }

    if (!requireUnique && solver.state.solved()) {
      solver.solutionCount = 1;
      solver.firstSolution = solver.state.board;
    }
    else {
      solver.Search(0);
    }

    result.solved = (solver.solutionCount >= 1);
    result.solutionCount = solver.solutionCount;
    result.stats = solver.stats;
    result.techniques = solver.techniques;
    if (solver.solutionCount >= 1)
      result.solution.cells = solver.firstSolution;

    return result;
  }

} // namespace sudoku::internal
