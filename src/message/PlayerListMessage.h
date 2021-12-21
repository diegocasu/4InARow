#ifndef INC_4INAROW_PLAYERLISTMESSAGE_H
#define INC_4INAROW_PLAYERLISTMESSAGE_H

#include <ostream>
#include <string>
#include <Constants.h>
#include "Message.h"

namespace fourinarow {

/**
 * Class representing a <code>PLAYER_LIST</code> message.
 */
class PlayerListMessage : public Message {
    private:
        uint8_t type = PLAYER_LIST;
        std::string playerList;
    public:
        PlayerListMessage() = default;
        explicit PlayerListMessage(std::string playerList);

        /**
         * Destroys the message and securely wipes its content from memory.
         */
        ~PlayerListMessage() override;

        PlayerListMessage(PlayerListMessage&&) = default;
        PlayerListMessage(const PlayerListMessage&) = default;
        PlayerListMessage& operator=(const PlayerListMessage&) = default;
        PlayerListMessage& operator=(PlayerListMessage&&) = default;

        uint8_t getType() const;
        const std::string& getPlayerList() const;

        std::vector<unsigned char> serialize() const override;
        void deserialize(const std::vector<unsigned char> &message) override;
};

}

std::ostream& operator<<(std::ostream &ostream, const fourinarow::PlayerListMessage &playerListMessage);

#endif //INC_4INAROW_PLAYERLISTMESSAGE_H
