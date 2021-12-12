#ifndef INC_4INAROW_ENDHANDSHAKE_H
#define INC_4INAROW_ENDHANDSHAKE_H

#include <Constants.h>
#include "Message.h"

namespace fourinarow {

/**
 * Class representing a END_HANDSHAKE message.
 */
class EndHandshake : public Message {
    private:
        uint8_t type = END_HANDSHAKE;
        std::vector<unsigned char> digitalSignature;

        /**
         * Checks if the message has the expected format for serialization.
         * @throws SerializationException  if the message has not the expected format.
         */
        void checkIfSerializable() const;
    public:
        EndHandshake() = default;
        explicit EndHandshake(std::vector<unsigned char> digitalSignature);
        ~EndHandshake() override = default;

        EndHandshake(EndHandshake &&) = default;
        EndHandshake(const EndHandshake&) = default;
        EndHandshake& operator=(const EndHandshake&) = default;
        EndHandshake& operator=(EndHandshake&&) = default;

        uint8_t getType() const;
        const std::vector<unsigned char>& getDigitalSignature() const;

        std::vector<unsigned char> serialize() const override;
        void deserialize(const std::vector<unsigned char> &message) override;
};

}

std::ostream& operator<<(std::ostream &ostream, const fourinarow::EndHandshake &endHandshake);

#endif //INC_4INAROW_ENDHANDSHAKE_H
