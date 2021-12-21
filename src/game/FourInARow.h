#ifndef INC_4INAROW_FOURINAROW_H
#define INC_4INAROW_FOURINAROW_H

#include <cstdint>
#include <vector>
#include <string>

namespace fourinarow {

/**
 * Class representing a "Four-in-a-row" game board.
 * It allows to register the moves of two players checking for their validity,
 * and to detect the end of the game. The board is represented as a matrix
 * in a bottom-up way, namely the (0,0) element is the bottom-left space of the board.
 */
class FourInARow {
    public:
        enum class Result { WIN, DRAW, LOSS };
    private:
        static const int EMPTY_SPACE = 0;
        static const int OPPONENT_DISC = -1;
        static const int MY_DISC = 1;

        bool matchFinished;
        Result result;
        unsigned int turn;
        std::vector<std::vector<int>> board;
        std::string opponent;

        /**
         * Checks if the given move is valid. A move is valid if:
         * 1) the match has not ended yet;
         * 2) the column affected by the insertion exists;
         * 3) the column affected by the insertion has at least one empty space.
         * @param columnIndex  the index of the column affected by the move.
         * @return             true if the move is valid, false otherwise.
         */
        bool isValidMove(const uint8_t &columnIndex) const;

        /**
         * Checks if a vertical line of four aligned discs has been achieved by the player.
         * The check is performed starting from the given disc position and examining
         * the three rows below. If the given disc position is outside the board,
         * the method returns false.
         * @param rowIndex     the row index of the disc.
         * @param columnIndex  the column index of the disc.
         * @param opponent     true if the player is the opponent, false otherwise.
         * @return             true if a vertical line of four aligned discs has been achieved by
         *                     the player, false otherwise.
         */
        bool checkWinOnVerticalLine(const uint8_t &rowIndex, const uint8_t &columnIndex, bool opponentMove) const;

        /**
         * Checks if a horizontal line of four aligned discs has been achieved by the player.
         * The check is performed examining all the possible horizontal lines of four discs
         * that include the given disc. If the given disc position is outside the board,
         * the method returns false.
         * @param rowIndex     the row index of the disc.
         * @param columnIndex  the column index of the disc.
         * @param opponent     true if the player is the opponent, false otherwise.
         * @return             true if a horizontal line of four aligned discs has been achieved by
         *                     the player, false otherwise.
         */
        bool checkWinOnHorizontalLine(const uint8_t &rowIndex, const uint8_t &columnIndex, bool opponentMove) const;

        /**
         * Checks if a left diagonal line of four aligned discs has been achieved by the player.
         * The check is performed examining all the possible left diagonal lines of four discs
         * that include the given disc. If the given disc position is outside the board,
         * the method returns false.
         * @param rowIndex     the row index of the disc.
         * @param columnIndex  the column index of the disc.
         * @param opponent     true if the player is the opponent, false otherwise.
         * @return             true if a left diagonal line of four aligned discs has been achieved by
         *                     the player, false otherwise.
         */
        bool checkWinOnLeftDiagonalLine(const uint8_t &rowIndex, const uint8_t &columnIndex, bool opponentMove) const;

        /**
         * Checks if a right diagonal line of four aligned discs has been achieved by the player.
         * The check is performed examining all the possible right diagonal lines of four discs
         * that include the given disc. If the given disc position is outside the board,
         * the method returns false.
         * @param rowIndex     the row index of the disc.
         * @param columnIndex  the column index of the disc.
         * @param opponent     true if the player is the opponent, false otherwise.
         * @return             true if a right diagonal line of four aligned discs has been achieved by
         *                     the player, false otherwise.
         */
        bool checkWinOnRightDiagonalLine(const uint8_t &rowIndex, const uint8_t &columnIndex, bool opponentMove) const;
    public:
        explicit FourInARow(std::string opponent);

        /**
         * Destroys the object and securely wipes the information about the match from memory.
         */
        ~FourInARow();

        FourInARow(FourInARow&&) = default;
        FourInARow& operator=(FourInARow&&) = default;
        FourInARow(const FourInARow&) = default;
        FourInARow& operator=(const FourInARow&) = default;

        /**
         * Returns a flag signaling if the match has ended or not. A match ends
         * when either one of the two players is able to achieve a
         * vertical/horizontal/diagonal line of four discs, or the board is full.
         * @return  true if the match has ended, false otherwise.
         */
        bool isMatchFinished() const;

        Result getResult() const;
        unsigned int getTurn() const;
        const std::string& getOpponent() const;

        /**
         * Registers a move in the board, checking its validity. A move is valid if:
         * 1) the match has not ended yet;
         * 2) the column affected by the insertion exists;
         * 3) the column affected by the insertion has at least one empty space.
         * The method can be used to register either a move of this player or
         * a move of the opponent. After a correct move has been registered,
         * <code>isMatchFinished()</code> can be used to check if the
         * match has ended, possibly retrieving the result with
         * <code>getResult()</code>.
         * @param columnIndex   the index of the column affected by the move.
         * @param opponentMove  true if the move is done by the opponent, false otherwise.
         * @return              true if the move was valid, false otherwise.
         */
        bool registerMove(const uint8_t &columnIndex, bool opponentMove);

        /**
         * Returns a string representing the board, the discs and the players.
         */
        std::string toString() const;
};

}

#endif //INC_4INAROW_FOURINAROW_H
