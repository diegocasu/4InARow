#ifndef INC_4INAROW_PLAYERLIST_H
#define INC_4INAROW_PLAYERLIST_H

#include <ostream>
#include <string>
#include <Constants.h>
#include "Message.h"

namespace fourinarow {

/**
 * Class representing a PLAYER_LIST message.
 */
class PlayerList : public Message {
    private:
        uint8_t type = PLAYER_LIST;
        std::string playerList;

        /**
         * Checks if the message has the expected format for serialization.
         * @throws SerializationException  if the message has not the expected format.
         */
        void checkIfSerializable();
    public:
        PlayerList() = default;
        explicit PlayerList(std::string playerList);

        /**
         * Destroys the message and securely wipes its content from memory.
         */
        ~PlayerList() override;

        PlayerList(PlayerList &&) = default;
        PlayerList(const PlayerList&) = default;
        PlayerList& operator=(const PlayerList&) = default;
        PlayerList& operator=(PlayerList&&) = default;

        uint8_t getType() const;
        const std::string& getPlayerList() const;

        std::vector<unsigned char> serialize() override;
        void deserialize(const std::vector<unsigned char> &message) override;
};

}

std::ostream& operator<<(std::ostream &ostream, const fourinarow::PlayerList &playerList);

#endif //INC_4INAROW_PLAYERLIST_H
