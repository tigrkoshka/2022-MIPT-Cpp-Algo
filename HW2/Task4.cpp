// https://leetcode.com/problems/sudoku-solver/
//
// Write a program to solve a Sudoku puzzle by filling the empty cells.
//
// A sudoku solution must satisfy all of the following rules:
//
// Each of the digits 1-9 must occur exactly once in each row.
// Each of the digits 1-9 must occur exactly once in each column.
// Each of the digits 1-9 must occur exactly once in each
// of the 9 3x3 sub-boxes of the grid.
//
// The '.' character indicates empty cells.

class Solution {
private:
  class Board {
  private:
    // Element presence manager
    //
    // Each mask represents a part of the board:
    // either a row, a column or a square 3x3
    class Mask {
    private:
      static uint getBit(char value) {
        return 1u << (value - '1');
      }

    public:
      // Register that the represented part of the board
      // contains the specified value
      void set(char value) {
        mask += getBit(value);
      }

      // Register that the represented part of the board
      // no longer contains the specified value
      void unset(char value) {
        mask -= getBit(value);
      }

      // Check whether the represented part of the board
      // contains the specified value
      [[nodiscard]] bool contains(char value) const {
        return (mask & getBit(value)) > 0;
      }

    private:
      // For performance reasons we aggregate the board's
      // rows, cols and squares info into uint variables,
      // so that setting, unsetting and checking values
      // can be done fast using bitwise operations
      //
      // The semantics is: for each value from 1..9
      // that are used in the game, the (value-1)-th bit
      // of mask identifies whether that value is set
      // somewhere in the part of the board that this mask represents
      uint mask{0};
    };

  private:
    // Get the 0..8 index of the 3x3 square
    // containing the specified position
    static size_t square(size_t row, size_t col) {
      return (row / 3) * 3 + (col / 3);
    }

    // Check whether the specified position is filled
    [[nodiscard]] bool isSet(size_t row, size_t col) const {
      return board[row][col] != noValue;
    }

    // Fill masks with the current board's filled values
    // For masks logics see above
    void fillMasks() {
      for (size_t row = 0; row < boardSize; ++row) {
        for (size_t col = 0; col < boardSize; ++col) {
          if (!isSet(row, col)) {
            continue;
          }

          set(row, col, board[row][col]);
        }
      }
    }

  public:
    explicit Board(vector<vector<char>> &board_) : board(board_) {
      fillMasks();
    }

    // Fill the specified position with the specified value
    void set(size_t row, size_t col, char value) {
      rows[row].set(value);
      cols[col].set(value);
      squares[square(row, col)].set(value);

      board[row][col] = value;
    }

    // Mark the specified position unfilled
    // Expects the position to be filled
    void unset(size_t row, size_t col) {
      char currValue = board[row][col];

      rows[row].unset(currValue);
      cols[col].unset(currValue);
      squares[square(row, col)].unset(currValue);

      board[row][col] = noValue;
    }

    // Check that the specified position can
    // be filled with the specified value, that is:
    //  - the specified row
    //  - the specified col
    //  - the deduced 3x3 square
    // do not contain the specified value
    [[nodiscard]] bool canSet(size_t row, size_t col, char value) const {
      return !rows[row].contains(value) &&
             !cols[col].contains(value) &&
             !squares[square(row, col)].contains(value);
    }

    // Get the next (in right-down direction) yet unfilled position
    //
    // Return format: row, col, ok,
    // where ok identifies whether any unfilled position was found
    [[nodiscard]] tuple<size_t, size_t, bool>
    getNextEmpty(size_t row, size_t col) const {
      // cannot perform a simple double loop over row and col,
      // because we need to check all columns in the rows below
      // the current one, but only columns to the right from
      // the current one in the current row
      while (row < boardSize && isSet(row, col)) {
        // imitate a double for loop
        if (col < boardSize) {
          ++col;
        } else {
          ++row;
          col = 0;
        }
      }

      // if the whole board was scanned and
      // no unfilled position was found,
      // return 0, 0, false
      if (row == boardSize) {
        return {};
      }

      return {row, col, true};
    }

  private:
    static constexpr size_t boardSize = 9;
    static constexpr char noValue = '.';

    // Invariant:
    //
    // if board[row][col] is filled with value,
    // then the following calls return true:
    //  - rows[row].contains(value)
    //  - cols[col].contains(value)
    //  - squares[square(row, col)].contains(value)
    //
    // and vice versa: if board[row][col] == noValue,
    // then the same calls return false

    vector<vector<char>> &board;

    array<Mask, boardSize> rows{Mask()};
    array<Mask, boardSize> cols{Mask()};
    array<Mask, boardSize> squares{Mask()};
  };

private:
  // Starts with the specified position and moves only
  // right and down trying to solve the board
  //
  // Returns a bool identifying whether the specified board is solvable
  static bool solveFromPosition(size_t i, size_t j, Board &board) {
    // get the next empty position to set
    auto[row, col, ok] = board.getNextEmpty(i, j);
    if (!ok) {
      return true;
    }

    // try setting all possible values to the found empty position
    for (char value = '1'; value <= '9'; ++value) {
      // if the position cannot be filled with the current value
      // due to the rules of the game, move on to the next position
      if (!board.canSet(row, col, value)) {
        continue;
      }

      // fill the position with the current value
      board.set(row, col, value);
      // try solving with the current position
      // filled with the current value
      bool success = solveFromPosition(row, col, board);
      // if the board can be solved with the current value
      // at the current position, report success
      if (success) {
        return true;
      }
      // else move on to the next value
      board.unset(row, col);
    }

    // if no value proved suitable or there is
    // no value that can be set to the current position
    // with the board remaining solvable,
    // report that the current board is unsolvable
    return false;
  }

public:
  static void solveSudoku(vector<vector<char>> &board) {
    Board b(board);
    solveFromPosition(0, 0, b);
  }
};