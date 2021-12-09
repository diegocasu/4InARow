#include <string.h>
#include <SerializationException.h>
#include <Utils.h>
#include "Player1Hello.h"

namespace fourinarow {

Player1Hello::Player1Hello(std::vector<unsigned char> nonce, std::vector<unsigned char> publicKey)
: nonce(std::move(nonce)), publicKey(std::move(publicKey)) {}

uint8_t Player1Hello::getType() const {
    return type;
}

const std::vector<unsigned char> &Player1Hello::getNonce() const {
    return nonce;
}

const std::vector<unsigned char> &Player1Hello::getPublicKey() const {
    return publicKey;
}

void Player1Hello::checkIfSerializable() {
    if (nonce.size() != NONCE_SIZE) {
        std::string errorMessage("The nonce size must be exactly ");
        errorMessage.append(std::to_string(NONCE_SIZE));
        errorMessage.append(" bytes. Nonce size: ");
        errorMessage.append(std::to_string(nonce.size()));
        errorMessage.append(" bytes");
        throw SerializationException(errorMessage);
    }

    if (publicKey.size() != PUBLIC_KEY_SIZE) {
        std::string errorMessage("The public key size must be exactly ");
        errorMessage.append(std::to_string(PUBLIC_KEY_SIZE));
        errorMessage.append(" bytes. Public key size: ");
        errorMessage.append(std::to_string(publicKey.size()));
        errorMessage.append(" bytes");
        throw SerializationException(errorMessage);
    }
}

std::vector<unsigned char> Player1Hello::serialize() {
    checkIfSerializable();

    size_t processedBytes = 0;
    size_t outputSize = sizeof(type) + nonce.size() + publicKey.size();
    std::vector<unsigned char> message(outputSize);

    // Serialize the type.
    memcpy(message.data(), &type, sizeof(type));
    processedBytes += sizeof(type);

    // Serialize the nonce.
    memcpy(message.data() + processedBytes, nonce.data(), nonce.size());
    processedBytes += nonce.size();

    // Serialize the public key.
    memcpy(message.data() + processedBytes, publicKey.data(), publicKey.size());

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
    processedBytes += NONCE_SIZE;

    // Deserialize the public key.
    checkIfEnoughSpace(message, processedBytes, PUBLIC_KEY_SIZE);
    publicKey.resize(PUBLIC_KEY_SIZE);
    memcpy(publicKey.data(), message.data() + processedBytes, PUBLIC_KEY_SIZE);
}

}

std::ostream& operator<<(std::ostream &ostream, const fourinarow::Player1Hello &player1Hello) {
    ostream << "Player1Hello{" << std::endl;
    ostream << "type=" << fourinarow::convertMessageType(player1Hello.getType()) << ',' << std::endl;
    ostream << "nonce=" << std::endl << fourinarow::dumpVector(player1Hello.getNonce());
    ostream << "publicKey=" << std::endl << fourinarow::dumpVector(player1Hello.getPublicKey());
    ostream << '}';
    return ostream;
}
