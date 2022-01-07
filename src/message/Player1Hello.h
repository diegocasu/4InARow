#ifndef INC_4INAROW_PLAYER1HELLO_H
#define INC_4INAROW_PLAYER1HELLO_H

#include <ostream>
#include <Constants.h>
#include "Message.h"

namespace fourinarow {

/**
 * Class representing a <code>PLAYER1_HELLO</code> message.
 */
class Player1Hello : public Message {
    private:
        uint8_t type = PLAYER1_HELLO;
        std::vector<unsigned char> nonce;
    public:
        Player1Hello() = default;
        explicit Player1Hello(std::vector<unsigned char> nonce);
        ~Player1Hello() override = default;

        Player1Hello(Player1Hello&&) = default;
        Player1Hello(const Player1Hello&) = default;
        Player1Hello& operator=(const Player1Hello&) = default;
        Player1Hello& operator=(Player1Hello&&) = default;

        uint8_t getType() const;
        const std::vector<unsigned char>& getNonce() const;

        std::vector<unsigned char> serialize() const override;
        void deserialize(const std::vector<unsigned char> &message) override;
};

}

std::ostream& operator<<(std::ostream &ostream, const fourinarow::Player1Hello &player1Hello);

#endif //INC_4INAROW_PLAYER1HELLO_H
