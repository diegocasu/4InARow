#include <string.h>
#include <arpa/inet.h>
#include <SerializationException.h>
#include <Utils.h>
#include "Player.h"

namespace fourinarow {

Player::Player(std::string ipAddress, std::vector<unsigned char> publicKey, bool firstToPlay)
: ipAddress(std::move(ipAddress)), publicKey(std::move(publicKey)), firstToPlay(firstToPlay) {}

uint8_t fourinarow::Player::getType() const {
    return type;
}

const std::string &fourinarow::Player::getIpAddress() const {
    return ipAddress;
}

const std::vector<unsigned char> &fourinarow::Player::getPublicKey() const {
    return publicKey;
}

bool fourinarow::Player::isFirstToPlay() const {
    return firstToPlay;
}

void Player::checkIfSerializable() {
    sockaddr_in dummySockaddr;
    auto result = inet_pton(AF_INET, ipAddress.data(), &dummySockaddr.sin_addr);
    if (result == 0) {
        throw SerializationException("Invalid network address");
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

std::vector<unsigned char> Player::serialize() {
    checkIfSerializable();

    size_t processedBytes = 0;
    size_t outputSize = sizeof(type) + sizeof(MAX_IPV4_ADDRESS_SIZE) + ipAddress.size() +
                        publicKey.size() + sizeof(uint8_t);
    std::vector<unsigned char> message(outputSize);

    // Serialize the type.
    memcpy(message.data(), &type, sizeof(type));
    processedBytes += sizeof(type);

    // Serialize the IPv4 address and its length.
    uint8_t addressLength = ipAddress.size();
    memcpy(message.data() + processedBytes, &addressLength, sizeof(addressLength));
    processedBytes += sizeof(addressLength);

    memcpy(message.data() + processedBytes, ipAddress.data(), ipAddress.size());
    processedBytes += ipAddress.size();

    // Serialize the public key.
    memcpy(message.data() + processedBytes, publicKey.data(), publicKey.size());
    processedBytes += publicKey.size();

    // Serialize the boolean.
    uint8_t firstToPlayRepresentation = (firstToPlay ? 1 : 0);
    memcpy(message.data() + processedBytes, &firstToPlayRepresentation, sizeof(firstToPlayRepresentation));

    return message;
}

void Player::deserialize(const std::vector<unsigned char> &message) {
    size_t processedBytes = 0;

    // Check if the type matches the expected one.
    uint8_t receivedType;
    checkIfEnoughSpace(message, processedBytes, sizeof(receivedType));
    memcpy(&receivedType, message.data(), sizeof(receivedType));
    processedBytes += sizeof(receivedType);

    if (receivedType != PLAYER) {
        throw SerializationException("Malformed message");
    }

    // Deserialize the IPv4 address and its length.
    uint8_t addressLength;
    checkIfEnoughSpace(message, processedBytes, sizeof(addressLength));
    memcpy(&addressLength, message.data() + processedBytes, sizeof(addressLength));
    processedBytes += sizeof(addressLength);

    if (addressLength == 0) {
        throw SerializationException("Malformed message");
    }

    checkIfEnoughSpace(message, processedBytes, addressLength);
    ipAddress.resize(addressLength);
    memcpy(&ipAddress[0], message.data() + processedBytes, addressLength);
    processedBytes += addressLength;

    // Deserialize the public key.
    checkIfEnoughSpace(message, processedBytes, PUBLIC_KEY_SIZE);
    publicKey.resize(PUBLIC_KEY_SIZE);
    memcpy(publicKey.data(), message.data() + processedBytes, PUBLIC_KEY_SIZE);
    processedBytes += PUBLIC_KEY_SIZE;

    // Deserialize the boolean.
    uint8_t firstToPlayRepresentation;
    checkIfEnoughSpace(message, processedBytes, sizeof(firstToPlayRepresentation));
    memcpy(&firstToPlayRepresentation, message.data() + processedBytes, sizeof(firstToPlayRepresentation));
    firstToPlay = (firstToPlayRepresentation == 0 ? false : true);
}

}

std::ostream& operator<<(std::ostream &ostream, const fourinarow::Player &player) {
    ostream << "Player{" << std::endl;
    ostream << "type=" << fourinarow::convertMessageType(player.getType()) << ',' << std::endl;
    ostream << "ipAddress=" << player.getIpAddress() << ',' << std::endl;
    ostream << "publicKey=" << std::endl << fourinarow::dumpVector(player.getPublicKey());
    ostream << "firstToPlay=" << std::boolalpha << player.isFirstToPlay() << std::noboolalpha << std::endl;
    ostream << '}';
    return ostream;
}