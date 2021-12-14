#include <string.h>
#include <arpa/inet.h>
#include <SerializationException.h>
#include <Utils.h>
#include "PlayerListMessage.h"

namespace fourinarow {

PlayerListMessage::PlayerListMessage(std::string playerList) : playerList(std::move(playerList)) {}

PlayerListMessage::~PlayerListMessage() {
    cleanse(type);
    cleanse(playerList);
}

uint8_t PlayerListMessage::getType() const {
    return type;
}

const std::string& PlayerListMessage::getPlayerList() const {
    return playerList;
}

std::vector<unsigned char> PlayerListMessage::serialize() const {
    checkPlayerListSize<SerializationException>(playerList);

    size_t processedBytes = 0;
    size_t outputSize = sizeof(type) + sizeof(MAX_PLAYER_LIST_SIZE) + playerList.size();
    std::vector<unsigned char> message(outputSize);

    // Serialize the type.
    memcpy(message.data(), &type, sizeof(type));
    processedBytes += sizeof(type);

    // Serialize the player list and its length.
    uint16_t playerListLength = htons(playerList.size());
    memcpy(message.data() + processedBytes, &playerListLength, sizeof(playerListLength));
    processedBytes += sizeof(playerListLength);

    if (playerList.empty())
        return message;

    memcpy(message.data() + processedBytes, playerList.data(), playerList.size());
    return message;
}

void PlayerListMessage::deserialize(const std::vector<unsigned char> &message) {
    size_t processedBytes = 0;

    // Check if the type matches the expected one.
    uint8_t receivedType;
    checkIfEnoughSpace(message, processedBytes, sizeof(receivedType));
    memcpy(&receivedType, message.data(), sizeof(receivedType));
    processedBytes += sizeof(receivedType);

    if (receivedType != PLAYER_LIST) {
        throw SerializationException("Malformed message");
    }

    // Deserialize the player list and its length.
    uint16_t playerListLength;
    checkIfEnoughSpace(message, processedBytes, sizeof(playerListLength));
    memcpy(&playerListLength, message.data() + processedBytes, sizeof(playerListLength));
    playerListLength = ntohs(playerListLength);
    processedBytes += sizeof(playerListLength);

    if (playerListLength == 0) {
        return;
    }

    checkIfEnoughSpace(message, processedBytes, playerListLength);
    playerList.resize(playerListLength);
    memcpy(&playerList[0], message.data() + processedBytes, playerListLength);
}

}

std::ostream& operator<<(std::ostream &ostream, const fourinarow::PlayerListMessage &playerListMessage) {
    ostream << "PlayerList{" << std::endl;
    ostream << "type=" << fourinarow::convertMessageType(playerListMessage.getType()) << ',' << std::endl;
    ostream << "playerList=" << playerListMessage.getPlayerList();
    ostream << '}';
    return ostream;
}
