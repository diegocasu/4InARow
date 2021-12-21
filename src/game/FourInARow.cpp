#include <iostream>
#include <Constants.h>
#include <Utils.h>
#include "FourInARow.h"

namespace fourinarow {

FourInARow::FourInARow(std::string opponent)
    : matchFinished(false),
      result(Result::LOSS),
      turn(0),
      board(ROWS, std::vector<int>(COLUMNS, EMPTY_SPACE)),
      opponent(std::move(opponent)) {}

FourInARow::~FourInARow() {
    for (auto &row: board) {
        cleanse(row);
    }
    cleanse(opponent);
    cleanse(matchFinished);
    cleanse(turn);
    cleanse(result);
}

bool FourInARow::isMatchFinished() const {
    return matchFinished;
}

FourInARow::Result FourInARow::getResult() const {
    return result;
}

unsigned int FourInARow::getTurn() const {
    return turn;
}

const std::string& FourInARow::getOpponent() const {
    return opponent;
}

bool FourInARow::isValidMove(const uint8_t &columnIndex) const {
    return !matchFinished
           && ROWS > 0
           && COLUMNS > 0
           && (columnIndex < COLUMNS)
           && (board[ROWS - 1][columnIndex] == EMPTY_SPACE);
}

bool FourInARow::checkWinOnVerticalLine(const uint8_t &rowIndex, const uint8_t &columnIndex, bool opponentMove) const {
    if (rowIndex >= ROWS || columnIndex >= COLUMNS || rowIndex < 3) {
        return false;
    }

    for (int i = rowIndex; i >= rowIndex - 3; i--) {
        if ((!opponentMove && board[i][columnIndex] == OPPONENT_DISC)
            || (opponentMove && board[i][columnIndex] == MY_DISC)) {
            return false;
        }

        if (board[i][columnIndex] == EMPTY_SPACE) {
            std::cerr << "Implementation error: empty space under disc (";
            std::cerr << unsigned(rowIndex) << ',' << unsigned(columnIndex) << ')' << std::endl;
            return false;
        }
    }

    return true;
}

bool FourInARow::checkWinOnHorizontalLine(const uint8_t &rowIndex, const uint8_t &columnIndex, bool opponentMove) const {
    if (rowIndex >= ROWS || columnIndex >= COLUMNS) {
        return false;
    }

    /*
     * Examine all the possible horizontal lines of four discs including the given disc.
     * The approach is based on a horizontal sliding window of size N=4. At the beginning, the window
     * starts on position (rowIndex, columnIndex - 3) and ends on position (rowIndex, columnIndex);
     * then, the starting point gets its column index increased by one every time a non-winning combination
     * is found. The last sliding window starts at (rowIndex, columnIndex) and ends at (rowIndex, columnIndex + 3).
     */
    for (int i = columnIndex - 3; i <= columnIndex; i++) {
        if (i < 0) { // Invalid sliding window.
            continue;
        }

        if (i + 3 >= COLUMNS) { // From this point on, the sliding window cannot include four discs anymore.
            break;
        }

        auto valid = true;
        for (int j = i; j <= i + 3; j++) {
            if ((opponentMove && board[rowIndex][j] == OPPONENT_DISC)
                || (!opponentMove && board[rowIndex][j] == MY_DISC)) {
                continue;
            }
            valid = false;
            break;
        }

        if (valid) {
            return true;
        }
    }

    return false;
}

bool FourInARow::checkWinOnLeftDiagonalLine(const uint8_t &rowIndex, const uint8_t &columnIndex, bool opponentMove) const {
    if (rowIndex >= ROWS || columnIndex >= COLUMNS) {
        return false;
    }

    /*
     * Examine all the possible left diagonal lines of four discs including the given disc.
     * The approach is based on a diagonal sliding window of size N=4. At the beginning, the window
     * starts on position (rowIndex + 3, columnIndex - 3) and ends on position (rowIndex, columnIndex);
     * then, the starting point gets its row index decreased by one and its column index increased by one
     * every time a non-winning combination is found. The last sliding window
     * starts at (rowIndex, columnIndex) and ends at (rowIndex - 3, columnIndex + 3).
     */
    for (int i = rowIndex + 3, j = columnIndex - 3; i >= rowIndex && j <= columnIndex; i--, j++) {
        if (i >= ROWS || j < 0) { // Invalid sliding window.
            continue;
        }

        if (i - 3 < 0 || j + 3 >= COLUMNS) { // From this point on, the sliding window cannot include four discs anymore.
            break;
        }

        auto valid = true;
        for (int h = i, k = j; h >= i - 3 && k <= j + 3; h--, k++) {
            if ((opponentMove && board[h][k] == OPPONENT_DISC) || (!opponentMove && board[h][k] == MY_DISC)) {
                continue;
            }
            valid = false;
            break;
        }

        if (valid) {
            return true;
        }
    }

    return false;
}

bool FourInARow::checkWinOnRightDiagonalLine(const uint8_t &rowIndex, const uint8_t &columnIndex, bool opponentMove) const {
    if (rowIndex >= ROWS || columnIndex >= COLUMNS) {
        return false;
    }

    /*
     * Examine all the possible right diagonal lines of four discs including the given disc.
     * The approach is based on a diagonal sliding window of size N=4. At the beginning, the window
     * starts on position (rowIndex + 3, columnIndex + 3) and ends on position (rowIndex, columnIndex);
     * then, the starting point gets both its row and column indexes decreased by one
     * every time a non-winning combination is found. The last sliding window
     * starts at (rowIndex, columnIndex) and ends at (rowIndex - 3, columnIndex - 3).
     */
    for (int i = rowIndex + 3, j = columnIndex + 3; i >= rowIndex && j >= columnIndex; i--, j--) {
        if (i >= ROWS || j >= COLUMNS) { // Invalid sliding window.
            continue;
        }

        if (i - 3 < 0 || j - 3 < 0) { // From this point on, the sliding window cannot include four discs anymore.
            break;
        }

        auto valid = true;
        for (int h = i, k = j; h >= i - 3 && k >= j - 3; h--, k--) {
            if ((opponentMove && board[h][k] == OPPONENT_DISC) || (!opponentMove && board[h][k] == MY_DISC)) {
                continue;
            }
            valid = false;
            break;
        }

        if (valid) {
            return true;
        }
    }

    return false;
}

bool FourInARow::registerMove(const uint8_t &columnIndex, bool opponentMove) {
    if (!isValidMove(columnIndex))
        return false;

    turn++;

    /*
     * Find the first available empty space in the column (bottom-up view).
     * The above check ensures that there is at least one empty space in the column.
     */
    auto insertionRow = 0u;
    for (auto i = 0; i < ROWS; i++) {
        if (board[i][columnIndex] == EMPTY_SPACE) {
            insertionRow = i;
            board[i][columnIndex] = opponentMove ? OPPONENT_DISC : MY_DISC;
            break;
        }
    }

    if (checkWinOnVerticalLine(insertionRow, columnIndex, opponentMove)
        || checkWinOnHorizontalLine(insertionRow, columnIndex, opponentMove)
        || checkWinOnLeftDiagonalLine(insertionRow, columnIndex, opponentMove)
        || checkWinOnRightDiagonalLine(insertionRow, columnIndex, opponentMove)) {
        matchFinished = true;
        result = opponentMove ? Result::LOSS : Result::WIN;
        return true;
    }

    if (turn == ROWS * COLUMNS) {
        matchFinished = true;
        result = Result::DRAW;
    }

    return true;
}

std::string FourInARow::toString() const {
    std::string boardPrint;

    for (int i = ROWS - 1; i >= 0; i--) {
        for (auto &j : board[i]) {
            if (j == EMPTY_SPACE) {
                boardPrint += "|   ";
                continue;
            }
            if (j == MY_DISC) {
                boardPrint += "| O ";
                continue;
            }
            if (j == OPPONENT_DISC) {
                boardPrint += "| X ";
                continue;
            }
        }
        boardPrint += "|\n";
    }

    for (int i = 0; i < COLUMNS; i++) {
        if (i == (COLUMNS - 1)) {
            boardPrint += "-----\n";
        } else {
            boardPrint += "----";
        }
    }

    /*
     * Print the column index. This works only
     * if numberOfColumns <= 9, otherwise the
     * indexes come out not aligned.
     */
    for (auto i = 0; i < COLUMNS; i++) {
        boardPrint += ("  " + std::to_string(i) + " ");
    }

    boardPrint += ("\n\nYou: O   " + opponent + ": X   Turn: " + std::to_string(turn)) + "\n";
    return boardPrint;
}

}
