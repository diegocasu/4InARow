#ifndef INC_4INAROW_MOVE_H
#define INC_4INAROW_MOVE_H

#include <ostream>
#include <Constants.h>
#include "Message.h"

namespace fourinarow {

/**
 * Class representing a MOVE message.
 */
class Move : public Message {
    private:
        uint8_t type = MOVE;
        uint8_t row = 0;
        uint8_t column = 0;

        /**
         * Checks if the given row number is a valid one.
         * @param candidateRow  the row number.
         * @return              true if the row number is valid, false otherwise.
         */
        bool isRowValid(uint8_t candidateRow);

        /**
         * Checks if the given column number is a valid one.
         * @param candidateColumn  the column number.
         * @return                 true if the column number is valid, false otherwise.
         */
        bool isColumnValid(uint8_t candidateColumn);

        /**
         * Checks if the message has the expected format for serialization.
         * @throws SerializationException  if the message has not the expected format.
         */
        void checkIfSerializable();
    public:
        Move() = default;
        Move(uint8_t row, uint8_t column);
        ~Move() override = default;

        Move(Move &&) = default;
        Move(const Move&) = default;
        Move& operator=(const Move&) = default;
        Move& operator=(Move&&) = default;

        uint8_t getType() const;
        uint8_t getRow() const;
        uint8_t getColumn() const;

        std::vector<unsigned char> serialize() override;
        void deserialize(const std::vector<unsigned char> &message) override;
};

}

std::ostream& operator<<(std::ostream &ostream, const fourinarow::Move &move);

#endif //INC_4INAROW_MOVE_H
