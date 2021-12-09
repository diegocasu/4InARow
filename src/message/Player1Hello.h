#ifndef INC_4INAROW_PLAYER1HELLO_H
#define INC_4INAROW_PLAYER1HELLO_H

#include <ostream>
#include <Constants.h>
#include "Message.h"

namespace fourinarow {

/**
 * Class representing a PLAYER1_HELLO message.
 */
class Player1Hello : public Message {
    private:
        uint8_t type = PLAYER1_HELLO;
        std::vector<unsigned char> nonce;
        std::vector<unsigned char> publicKey;

        /**
         * Checks if the message has the expected format for serialization.
         * @throws SerializationException  if the message has not the expected format.
         */
        void checkIfSerializable();
    public:
        Player1Hello() = default;
        Player1Hello(std::vector<unsigned char> nonce, std::vector<unsigned char> publicKey);
        ~Player1Hello() override = default;

        Player1Hello(Player1Hello &&) = default;
        Player1Hello(const Player1Hello&) = default;
        Player1Hello& operator=(const Player1Hello&) = default;
        Player1Hello& operator=(Player1Hello&&) = default;

        uint8_t getType() const;
        const std::vector<unsigned char> &getNonce() const;
        const std::vector<unsigned char> &getPublicKey() const;

        std::vector<unsigned char> serialize() override;
        void deserialize(const std::vector<unsigned char> &message) override;
};

}

std::ostream& operator<<(std::ostream &ostream, const fourinarow::Player1Hello &player1Hello);

#endif //INC_4INAROW_PLAYER1HELLO_H
