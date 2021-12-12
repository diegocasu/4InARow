#ifndef INC_4INAROW_CLIENTHELLO_H
#define INC_4INAROW_CLIENTHELLO_H

#include <ostream>
#include <string>
#include <Constants.h>
#include "Message.h"

namespace fourinarow {

/**
 * Class representing a CLIENT_HELLO message.
 */
class ClientHello : public Message {
    private:
        uint8_t type = CLIENT_HELLO;
        std::string username;
        std::vector<unsigned char> nonce;
        std::vector<unsigned char> publicKey;

        /**
         * Checks if the message has the expected format for serialization.
         * @throws SerializationException  if the message has not the expected format.
         */
        void checkIfSerializable() const;
    public:
        ClientHello() = default;
        ClientHello(std::string username, std::vector<unsigned char> nonce, std::vector<unsigned char> publicKey);
        ~ClientHello() override = default;

        ClientHello(ClientHello &&) = default;
        ClientHello(const ClientHello&) = default;
        ClientHello& operator=(const ClientHello&) = default;
        ClientHello& operator=(ClientHello&&) = default;

        uint8_t getType() const;
        const std::string& getUsername() const;
        const std::vector<unsigned char>& getNonce() const;
        const std::vector<unsigned char>& getPublicKey() const;

        std::vector<unsigned char> serialize() const override;
        void deserialize(const std::vector<unsigned char> &message) override;
};

}

std::ostream& operator<<(std::ostream &ostream, const fourinarow::ClientHello &clientHello);

#endif //INC_4INAROW_CLIENTHELLO_H
