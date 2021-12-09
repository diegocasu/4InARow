#ifndef INC_4INAROW_SERVERHELLO_H
#define INC_4INAROW_SERVERHELLO_H

#include <ostream>
#include <Constants.h>
#include "Message.h"

namespace fourinarow {

/**
 * Class representing a SERVER_HELLO message.
 */
class ServerHello : public Message {
    private:
        uint8_t type = SERVER_HELLO;
        std::vector<unsigned char> certificate;
        std::vector<unsigned char> nonce;
        std::vector<unsigned char> publicKey;
        std::vector<unsigned char> digitalSignature;

        /**
         * Checks if the message has the expected format for serialization.
         * @throws SerializationException  if the message has not the expected format.
         */
        void checkIfSerializable();
    public:
        ServerHello() = default;
        ServerHello(std::vector<unsigned char> certificate,
                    std::vector<unsigned char> nonce,
                    std::vector<unsigned char> publicKey,
                    std::vector<unsigned char> digitalSignature);
        ~ServerHello() override = default;

        ServerHello(ServerHello &&) = default;
        ServerHello(const ServerHello&) = default;
        ServerHello& operator=(const ServerHello&) = default;
        ServerHello& operator=(ServerHello&&) = default;

        uint8_t getType() const;
        const std::vector<unsigned char>& getCertificate() const;
        const std::vector<unsigned char>& getNonce() const;
        const std::vector<unsigned char>& getPublicKey() const;
        const std::vector<unsigned char>& getDigitalSignature() const;

        std::vector<unsigned char> serialize() override;
        void deserialize(const std::vector<unsigned char> &message) override;
};

}

std::ostream& operator<<(std::ostream &ostream, const fourinarow::ServerHello &serverHello);

#endif //INC_4INAROW_SERVERHELLO_H