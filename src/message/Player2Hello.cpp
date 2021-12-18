#include <string.h>
#include <SerializationException.h>
#include <Utils.h>
#include "Player2Hello.h"

namespace fourinarow {

Player2Hello::Player2Hello(std::vector<unsigned char> nonce,
                           std::vector<unsigned char> publicKey,
                           std::vector<unsigned char> digitalSignature)
: nonce(std::move(nonce)), publicKey(std::move(publicKey)), digitalSignature(std::move(digitalSignature)) {}

uint8_t Player2Hello::getType() const {
    return type;
}

const std::vector<unsigned char>& Player2Hello::getNonce() const {
    return nonce;
}

const std::vector<unsigned char>& Player2Hello::getPublicKey() const {
    return publicKey;
}

const std::vector<unsigned char>& Player2Hello::getDigitalSignature() const {
    return digitalSignature;
}

std::vector<unsigned char> Player2Hello::serialize() const {
    checkNonceSize<SerializationException>(nonce);
    checkEcdhPublicKeySize<SerializationException>(publicKey);
    checkDigitalSignatureSize<SerializationException>(digitalSignature);

    size_t processedBytes = 0;
    size_t outputSize = sizeof(type) + nonce.size() + publicKey.size() + digitalSignature.size();
    std::vector<unsigned char> message(outputSize);

    // Serialize the type.
    memcpy(message.data(), &type, sizeof(type));
    processedBytes += sizeof(type);

    // Serialize the nonce.
    memcpy(message.data() + processedBytes, nonce.data(), nonce.size());
    processedBytes += nonce.size();

    // Serialize the public key.
    memcpy(message.data() + processedBytes, publicKey.data(), publicKey.size());
    processedBytes += publicKey.size();

    // Serialize the digital signature.
    memcpy(message.data() + processedBytes, digitalSignature.data(), digitalSignature.size());

    return message;
}

void Player2Hello::deserialize(const std::vector<unsigned char> &message) {
    size_t processedBytes = 0;

    // Check if the type matches the expected one.
    uint8_t receivedType;
    checkIfEnoughSpace(message, processedBytes, sizeof(receivedType));
    memcpy(&receivedType, message.data(), sizeof(receivedType));
    processedBytes += sizeof(receivedType);

    if (receivedType != PLAYER2_HELLO) {
        throw SerializationException("Malformed message");
    }

    // Deserialize the nonce.
    checkIfEnoughSpace(message, processedBytes, NONCE_SIZE);
    nonce.resize(NONCE_SIZE);
    memcpy(nonce.data(), message.data() + processedBytes, NONCE_SIZE);
    processedBytes += NONCE_SIZE;

    // Deserialize the public key.
    checkIfEnoughSpace(message, processedBytes, ECDH_PUBLIC_KEY_SIZE);
    publicKey.resize(ECDH_PUBLIC_KEY_SIZE);
    memcpy(publicKey.data(), message.data() + processedBytes, ECDH_PUBLIC_KEY_SIZE);
    processedBytes += ECDH_PUBLIC_KEY_SIZE;

    // Deserialize the digital signature.
    checkIfEnoughSpace(message, processedBytes, DIGITAL_SIGNATURE_SIZE);
    digitalSignature.resize(DIGITAL_SIGNATURE_SIZE);
    memcpy(digitalSignature.data(), message.data() + processedBytes, DIGITAL_SIGNATURE_SIZE);
}

}

std::ostream& operator<<(std::ostream &ostream, const fourinarow::Player2Hello &player2Hello) {
    ostream << "Player2Hello{" << std::endl;
    ostream << "type=" << fourinarow::convertMessageType(player2Hello.getType()) << ',' << std::endl;
    ostream << "nonce=" << std::endl << fourinarow::dumpVector(player2Hello.getNonce());
    ostream << "publicKey=" << std::endl << fourinarow::dumpVector(player2Hello.getPublicKey());
    ostream << "digitalSignature=" << std::endl << fourinarow::dumpVector(player2Hello.getDigitalSignature());
    ostream << '}';
    return ostream;
}
