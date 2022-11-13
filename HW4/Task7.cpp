// https://leetcode.com/problems/number-of-islands/
//
// Given an m x n 2D binary grid which represents a map
// of '1's (land) and '0's (water), return the number of islands.
//
// An island is surrounded by water and is formed by connecting adjacent
// lands horizontally or vertically.
//
// You may assume all four edges of the grid are all surrounded by water.

class Solution {
private:
  // visitAllIsland marks all land reachable from (i, j) as water
  //
  // NOTE: that is not the best solution code-style-wise, because
  // the name of the numIslands function suggests that grid should be accepted
  // as a const reference, but since this is a Leetcode task, we will allow it,
  // because copying the grid would affect the performance dramatically
  static void visitAllIsland(size_t i, size_t j, vector<vector<char>>& grid) {
    if (grid[i][j] == '0') {
      return;
    }

    grid[i][j] = '0';

    if (i < grid.size() - 1) {
      visitAllIsland(i + 1, j, grid);
    }

    if (i > 0) {
      visitAllIsland(i - 1, j, grid);
    }

    if (j < grid[i].size() - 1) {
      visitAllIsland(i, j + 1, grid);
    }

    if (j > 0) {
      visitAllIsland(i, j - 1, grid);
    }
  }

public:
  static size_t numIslands(vector<vector<char>>& grid) {
    size_t nIslands = 0;

    for (size_t i = 0; i < grid.size(); ++i) {
      for (size_t j = 0; j < grid[i].size(); ++j) {
        // If the current cell is land, mark it and all adjacent land as sea

        if (grid[i][j] == '0') {
          continue;
        }

        ++nIslands;
        visitAllIsland(i, j, grid);
      }
    }

    return nIslands;
  }
};
