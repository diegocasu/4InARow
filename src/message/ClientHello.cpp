#include <string.h>
#include <SerializationException.h>
#include <Utils.h>
#include "ClientHello.h"

namespace fourinarow {

ClientHello::ClientHello(std::string username, std::vector<unsigned char> nonce, std::vector<unsigned char> publicKey)
: username(std::move(username)), nonce(std::move(nonce)), publicKey(std::move(publicKey)) {}

uint8_t ClientHello::getType() const {
    return type;
}

const std::string& ClientHello::getUsername() const {
    return username;
}

const std::vector<unsigned char>& ClientHello::getNonce() const {
    return nonce;
}

const std::vector<unsigned char>& ClientHello::getPublicKey() const {
    return publicKey;
}

void ClientHello::checkIfSerializable() {
    if (username.empty() || username.size() > MAX_USERNAME_SIZE) {
        throw SerializationException("The username size must be greater than zero, and less than or equal to " +
                                     std::to_string(MAX_USERNAME_SIZE) +
                                     " bytes. Username size: " +
                                     std::to_string(username.size()) +
                                     " bytes");
    }

    if (nonce.size() != NONCE_SIZE) {
        throw SerializationException("The nonce size must be exactly " +
                                     std::to_string(NONCE_SIZE) +
                                     " bytes. Nonce size: " +
                                     std::to_string(nonce.size()) +
                                     " bytes");
    }

    if (publicKey.size() != PUBLIC_KEY_SIZE) {
        throw SerializationException("The public key size must be exactly " +
                                     std::to_string(PUBLIC_KEY_SIZE) +
                                     " bytes. Public key size: " +
                                     std::to_string(publicKey.size()) +
                                     " bytes");
    }
}

std::vector<unsigned char> ClientHello::serialize() {
    checkIfSerializable();

    size_t processedBytes = 0;
    size_t outputSize = sizeof(type) + sizeof(MAX_USERNAME_SIZE) + username.size() + nonce.size() + publicKey.size();
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
    processedBytes += nonce.size();

    // Serialize the public key.
    memcpy(message.data() + processedBytes, publicKey.data(), publicKey.size());

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
    processedBytes += usernameLength;

    // Deserialize the nonce.
    checkIfEnoughSpace(message, processedBytes, NONCE_SIZE);
    nonce.resize(NONCE_SIZE);
    memcpy(nonce.data(), message.data() + processedBytes, NONCE_SIZE);
    processedBytes += NONCE_SIZE;

    // Deserialize the public key.
    checkIfEnoughSpace(message, processedBytes, PUBLIC_KEY_SIZE);
    publicKey.resize(PUBLIC_KEY_SIZE);
    memcpy(publicKey.data(), message.data() + processedBytes, PUBLIC_KEY_SIZE);
}

}

std::ostream& operator<<(std::ostream &ostream, const fourinarow::ClientHello &clientHello) {
    ostream << "ClientHello{" << std::endl;
    ostream << "type=" << fourinarow::convertMessageType(clientHello.getType()) << ',' << std::endl;
    ostream << "username=" << clientHello.getUsername() << ',' << std::endl;
    ostream << "nonce=" << std::endl << fourinarow::dumpVector(clientHello.getNonce());
    ostream << "publicKey=" << std::endl << fourinarow::dumpVector(clientHello.getPublicKey());
    ostream << '}';
    return ostream;
}
