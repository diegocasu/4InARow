#ifndef INC_4INAROW_CHALLENGE_H
#define INC_4INAROW_CHALLENGE_H

#include <ostream>
#include <string>
#include <Constants.h>
#include "Message.h"

namespace fourinarow {

/**
 * Class representing a CHALLENGE message.
 */
class Challenge : public Message {
    private:
        uint8_t type = CHALLENGE;
        std::string username;

        /**
         * Checks if the message has the expected format for serialization.
         * @throws SerializationException  if the message has not the expected format.
         */
        void checkIfSerializable() const;
    public:
        Challenge() = default;
        explicit Challenge(std::string username);

        /**
         * Destroys the message and securely wipes its content from memory.
         */
        ~Challenge() override;

        Challenge(Challenge &&) = default;
        Challenge(const Challenge&) = default;
        Challenge& operator=(const Challenge&) = default;
        Challenge& operator=(Challenge&&) = default;

        uint8_t getType() const;
        const std::string& getUsername() const;

        std::vector<unsigned char> serialize() const override;
        void deserialize(const std::vector<unsigned char> &message) override;
};

}

std::ostream& operator<<(std::ostream &ostream, const fourinarow::Challenge &challenge);

#endif //INC_4INAROW_CHALLENGE_H
