#ifndef INC_4INAROW_INFOMESSAGE_H
#define INC_4INAROW_INFOMESSAGE_H

#include <cstdint>
#include "Message.h"

namespace fourinarow {

/**
 * Class representing a message composed only of a type
 * (ex. REQ_PLAYER_LIST, PROTOCOL_VIOLATION, etc.).
 */
class InfoMessage : public Message {
    private:
        uint8_t type = 0;
    public:
        InfoMessage() = default;
        explicit InfoMessage(uint8_t type);

        /**
         * Destroys the message and securely wipes its content from memory.
         */
        ~InfoMessage() override;

        InfoMessage(InfoMessage &&) = default;
        InfoMessage(const InfoMessage&) = default;
        InfoMessage& operator=(const InfoMessage&) = default;
        InfoMessage& operator=(InfoMessage&&) = default;

        uint8_t getType() const;

        std::vector<unsigned char> serialize() const override;
        void deserialize(const std::vector<unsigned char> &message) override;
};

}

std::ostream& operator<<(std::ostream &ostream, const fourinarow::InfoMessage &infoMessage);

#endif //INC_4INAROW_INFOMESSAGE_H
