#ifndef INC_4INAROW_PLAYER2HELLO_H
#define INC_4INAROW_PLAYER2HELLO_H

#include <ostream>
#include <Constants.h>
#include "Message.h"

namespace fourinarow {

/**
 * Class representing a <code>PLAYER2_HELLO</code> message.
 */
class Player2Hello : public Message {
    private:
        uint8_t type = PLAYER2_HELLO;
        std::vector<unsigned char> nonce;
        std::vector<unsigned char> publicKey;
        std::vector<unsigned char> digitalSignature;
    public:
        Player2Hello() = default;
        Player2Hello(std::vector<unsigned char> nonce,
                     std::vector<unsigned char> publicKey,
                     std::vector<unsigned char> digitalSignature);
        ~Player2Hello() override = default;

        Player2Hello(Player2Hello&&) = default;
        Player2Hello(const Player2Hello&) = default;
        Player2Hello& operator=(const Player2Hello&) = default;
        Player2Hello& operator=(Player2Hello&&) = default;

        uint8_t getType() const;
        const std::vector<unsigned char>& getNonce() const;
        const std::vector<unsigned char>& getPublicKey() const;
        const std::vector<unsigned char>& getDigitalSignature() const;

        std::vector<unsigned char> serialize() const override;
        void deserialize(const std::vector<unsigned char> &message) override;
};

}

std::ostream& operator<<(std::ostream &ostream, const fourinarow::Player2Hello &player2Hello);

#endif //INC_4INAROW_PLAYER2HELLO_H
