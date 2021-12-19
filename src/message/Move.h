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
        uint8_t column = 0;
    public:
        Move() = default;
        explicit Move(uint8_t column);

        /**
         * Destroys the message and securely wipes its content from memory.
         */
        ~Move() override;

        Move(Move &&) = default;
        Move(const Move&) = default;
        Move& operator=(const Move&) = default;
        Move& operator=(Move&&) = default;

        uint8_t getType() const;
        uint8_t getColumn() const;

        std::vector<unsigned char> serialize() const override;
        void deserialize(const std::vector<unsigned char> &message) override;
};

}

std::ostream& operator<<(std::ostream &ostream, const fourinarow::Move &move);

#endif //INC_4INAROW_MOVE_H
