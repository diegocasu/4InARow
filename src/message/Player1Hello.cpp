#include <string.h>
#include <SerializationException.h>
#include <Utils.h>
#include "Player1Hello.h"

namespace fourinarow {

Player1Hello::Player1Hello(std::vector<unsigned char> nonce) : nonce(std::move(nonce)) {}

uint8_t Player1Hello::getType() const {
    return type;
}

const std::vector<unsigned char>& Player1Hello::getNonce() const {
    return nonce;
}

std::vector<unsigned char> Player1Hello::serialize() const {
    checkNonceSize<SerializationException>(nonce);

    size_t processedBytes = 0;
    size_t outputSize = sizeof(type) + nonce.size();
    std::vector<unsigned char> message(outputSize);

    // Serialize the type.
    memcpy(message.data(), &type, sizeof(type));
    processedBytes += sizeof(type);

    // Serialize the nonce.
    memcpy(message.data() + processedBytes, nonce.data(), nonce.size());

    return message;
}

void Player1Hello::deserialize(const std::vector<unsigned char> &message) {
    size_t processedBytes = 0;

    // Check if the type matches the expected one.
    uint8_t receivedType;
    checkIfEnoughSpace(message, processedBytes, sizeof(receivedType));
    memcpy(&receivedType, message.data(), sizeof(receivedType));
    processedBytes += sizeof(receivedType);

    if (receivedType != PLAYER1_HELLO) {
        throw SerializationException("Malformed message");
    }

    // Deserialize the nonce.
    checkIfEnoughSpace(message, processedBytes, NONCE_SIZE);
    nonce.resize(NONCE_SIZE);
    memcpy(nonce.data(), message.data() + processedBytes, NONCE_SIZE);
}

}

std::ostream& operator<<(std::ostream &ostream, const fourinarow::Player1Hello &player1Hello) {
    ostream << "Player1Hello{" << std::endl;
    ostream << "type=" << fourinarow::convertMessageType(player1Hello.getType()) << ',' << std::endl;
    ostream << "nonce=" << std::endl << fourinarow::dumpVector(player1Hello.getNonce());
    ostream << '}';
    return ostream;
}
