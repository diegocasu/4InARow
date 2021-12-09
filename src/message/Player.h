#ifndef INC_4INAROW_PLAYER_H
#define INC_4INAROW_PLAYER_H

#include <ostream>
#include <string>
#include <Constants.h>
#include "Message.h"

namespace fourinarow {

/**
 * Class representing a PLAYER message.
 */
class Player : public Message {
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
        Player() = default;
        Player(std::string ipAddress, std::vector<unsigned char> publicKey, bool firstToPlay);
        ~Player() override = default;

        Player(Player &&) = default;
        Player(const Player&) = default;
        Player& operator=(const Player&) = default;
        Player& operator=(Player&&) = default;

        uint8_t getType() const;
        const std::string& getIpAddress() const;
        const std::vector<unsigned char>& getPublicKey() const;
        bool isFirstToPlay() const;

        std::vector<unsigned char> serialize() override;
        void deserialize(const std::vector<unsigned char> &message) override;
};

}

std::ostream& operator<<(std::ostream &ostream, const fourinarow::Player &player);

#endif //INC_4INAROW_PLAYER_H
