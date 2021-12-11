#ifndef INC_4INAROW_PLAYERMESSAGE_H
#define INC_4INAROW_PLAYERMESSAGE_H

#include <ostream>
#include <string>
#include <Constants.h>
#include "Message.h"

namespace fourinarow {

/**
 * Class representing a PLAYER message.
 */
class PlayerMessage : public Message {
    private:
        uint8_t type = PLAYER;
        std::string ipAddress;
        std::vector<unsigned char> publicKey;
        bool firstToPlay = false;

        /**
         * Checks if the message has the expected format for serialization.
         * @throws SerializationException  if the message has not the expected format.
         */
        void checkIfSerializable();
    public:
        PlayerMessage() = default;
        PlayerMessage(std::string ipAddress, std::vector<unsigned char> publicKey, bool firstToPlay);

        /**
         * Destroys the message and securely wipes its content from memory.
         */
        ~PlayerMessage() override;

        PlayerMessage(PlayerMessage &&) = default;
        PlayerMessage(const PlayerMessage&) = default;
        PlayerMessage& operator=(const PlayerMessage&) = default;
        PlayerMessage& operator=(PlayerMessage&&) = default;

        uint8_t getType() const;
        const std::string& getIpAddress() const;
        const std::vector<unsigned char>& getPublicKey() const;
        bool isFirstToPlay() const;

        std::vector<unsigned char> serialize() override;
        void deserialize(const std::vector<unsigned char> &message) override;
};

}

std::ostream& operator<<(std::ostream &ostream, const fourinarow::PlayerMessage &playerMessage);

#endif //INC_4INAROW_PLAYERMESSAGE_H
