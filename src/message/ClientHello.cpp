#include <string.h>
#include <SerializationException.h>
#include <Utils.h>
#include "ClientHello.h"

namespace fourinarow {

ClientHello::ClientHello(std::string username, std::vector<unsigned char> nonce)
: username(std::move(username)), nonce(std::move(nonce)) {}

uint8_t ClientHello::getType() const {
    return type;
}

const std::string& ClientHello::getUsername() const {
    return username;
}

const std::vector<unsigned char>& ClientHello::getNonce() const {
    return nonce;
}

std::vector<unsigned char> ClientHello::serialize() const {
    checkUsernameValidity<SerializationException>(username);
    checkNonceSize<SerializationException>(nonce);

    size_t processedBytes = 0;
    size_t outputSize = sizeof(type) + sizeof(MAX_USERNAME_SIZE) + username.size() + nonce.size();
    std::vector<unsigned char> message(outputSize);

    // Serialize the type.
    memcpy(message.data(), &type, sizeof(type));
    processedBytes += sizeof(type);

    // Serialize the username and its length.
    uint8_t usernameLength = username.size();
    memcpy(message.data() + processedBytes, &usernameLength, sizeof(usernameLength));
    processedBytes += sizeof(usernameLength);

    memcpy(message.data() + processedBytes, username.data(), username.size());
    processedBytes += username.size();

    // Serialize the nonce.
    memcpy(message.data() + processedBytes, nonce.data(), nonce.size());

    return message;
}

void ClientHello::deserialize(const std::vector<unsigned char> &message) {
    size_t processedBytes = 0;

    // Check if the type matches the expected one.
    uint8_t receivedType;
    checkIfEnoughSpace(message, processedBytes, sizeof(receivedType));
    memcpy(&receivedType, message.data(), sizeof(receivedType));
    processedBytes += sizeof(receivedType);

    if (receivedType != CLIENT_HELLO) {
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
    processedBytes += usernameLength;

    // Deserialize the nonce.
    checkIfEnoughSpace(message, processedBytes, NONCE_SIZE);
    nonce.resize(NONCE_SIZE);
    memcpy(nonce.data(), message.data() + processedBytes, NONCE_SIZE);
}

}

std::ostream& operator<<(std::ostream &ostream, const fourinarow::ClientHello &clientHello) {
    ostream << "ClientHello{" << std::endl;
    ostream << "type=" << fourinarow::convertMessageType(clientHello.getType()) << ',' << std::endl;
    ostream << "username=" << clientHello.getUsername() << ',' << std::endl;
    ostream << "nonce=" << std::endl << fourinarow::dumpVector(clientHello.getNonce());
    ostream << '}';
    return ostream;
}
