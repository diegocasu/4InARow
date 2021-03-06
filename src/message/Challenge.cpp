#include <string.h>
#include <arpa/inet.h>
#include <SerializationException.h>
#include <Utils.h>
#include "Challenge.h"

namespace fourinarow {

Challenge::Challenge(std::string username) : username(std::move(username)) {}

Challenge::~Challenge() {
    cleanse(type);
    cleanse(username);
}

uint8_t fourinarow::Challenge::getType() const {
    return type;
}

const std::string& Challenge::getUsername() const {
    return username;
}

std::vector<unsigned char> Challenge::serialize() const {
    checkUsernameValidity<SerializationException>(username);

    size_t processedBytes = 0;
    size_t outputSize = sizeof(type) + sizeof(MAX_USERNAME_SIZE) + username.size();
    std::vector<unsigned char> message(outputSize);

    // Serialize the type.
    memcpy(message.data(), &type, sizeof(type));
    processedBytes += sizeof(type);

    // Serialize the username and its length.
    uint8_t usernameLength = username.size();
    memcpy(message.data() + processedBytes, &usernameLength, sizeof(usernameLength));
    processedBytes += sizeof(usernameLength);

    memcpy(message.data() + processedBytes, username.data(), username.size());

    return message;
}

void Challenge::deserialize(const std::vector<unsigned char> &message) {
    size_t processedBytes = 0;

    // Check if the type matches the expected one.
    uint8_t receivedType;
    checkIfEnoughSpace(message, processedBytes, sizeof(receivedType));
    memcpy(&receivedType, message.data(), sizeof(receivedType));
    processedBytes += sizeof(receivedType);

    if (receivedType != CHALLENGE) {
        throw SerializationException("Malformed message");
    }

    // Deserialize the username and its length.
    uint8_t usernameLength;
    checkIfEnoughSpace(message, processedBytes, sizeof(usernameLength));
    memcpy(&usernameLength, message.data() + processedBytes, sizeof(usernameLength));
    processedBytes += sizeof(usernameLength);

    if (usernameLength == 0) {
        throw SerializationException("Malformed message");
    }

    checkIfEnoughSpace(message, processedBytes, usernameLength);
    username.resize(usernameLength);
    memcpy(&username[0], message.data() + processedBytes, usernameLength);
    checkUsernameValidity<SerializationException>(username);
}

}

std::ostream& operator<<(std::ostream &ostream, const fourinarow::Challenge &challenge) {
    ostream << "Challenge{";
    ostream << "type=" << fourinarow::convertMessageType(challenge.getType()) << ", ";
    ostream << "username=" << challenge.getUsername();
    ostream << '}';
    return ostream;
}
