#include <string.h>
#include <arpa/inet.h>
#include <SerializationException.h>
#include <Utils.h>
#include "PlayerList.h"

namespace fourinarow {

PlayerList::PlayerList(std::string playerList) : playerList(std::move(playerList)) {}

uint8_t PlayerList::getType() const {
    return type;
}

const std::string& PlayerList::getPlayerList() const {
    return playerList;
}

void PlayerList::checkIfSerializable() {
    if (playerList.empty() || playerList.size() > MAX_PLAYER_LIST_SIZE) {
        std::string errorMessage("The player list size must be greater than zero, and less than or equal to ");
        errorMessage.append(std::to_string(MAX_PLAYER_LIST_SIZE));
        errorMessage.append(" bytes. Player list size: ");
        errorMessage.append(std::to_string(playerList.size()));
        errorMessage.append(" bytes");
        throw SerializationException(errorMessage);
    }
}

std::vector<unsigned char> PlayerList::serialize() {
    checkIfSerializable();

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

    memcpy(message.data() + processedBytes, playerList.data(), playerList.size());

    return message;
}

void PlayerList::deserialize(const std::vector<unsigned char> &message) {
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
        throw SerializationException("Malformed message");
    }

    checkIfEnoughSpace(message, processedBytes, playerListLength);
    playerList.resize(playerListLength);
    memcpy(&playerList[0], message.data() + processedBytes, playerListLength);
}

}

std::ostream& operator<<(std::ostream &ostream, const fourinarow::PlayerList &playerList) {
    ostream << "PlayerList{" << std::endl;
    ostream << "type=" << fourinarow::convertMessageType(playerList.getType()) << ',' << std::endl;
    ostream << "playerList=" << playerList.getPlayerList();
    ostream << '}';
    return ostream;
}
