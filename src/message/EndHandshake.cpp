#include <string.h>
#include <ostream>
#include <Utils.h>
#include <SerializationException.h>
#include "EndHandshake.h"

namespace fourinarow {

EndHandshake::EndHandshake(std::vector<unsigned char> publicKey, std::vector<unsigned char> digitalSignature)
: publicKey(std::move(publicKey)), digitalSignature(std::move(digitalSignature)) {}

uint8_t EndHandshake::getType() const {
    return type;
}

const std::vector<unsigned char>& EndHandshake::getPublicKey() const {
    return publicKey;
}

const std::vector<unsigned char>& EndHandshake::getDigitalSignature() const {
    return digitalSignature;
}

std::vector<unsigned char> EndHandshake::serialize() const {
    checkDigitalSignatureSize<SerializationException>(digitalSignature);
    checkEcdhPublicKeySize<SerializationException>(publicKey);

    size_t processedBytes = 0;
    size_t outputSize = sizeof(type) + publicKey.size() + digitalSignature.size();
    std::vector<unsigned char> message(outputSize);

    // Serialize the type.
    memcpy(message.data(), &type, sizeof(type));
    processedBytes += sizeof(type);

    // Serialize the public key.
    memcpy(message.data() + processedBytes, publicKey.data(), publicKey.size());
    processedBytes += publicKey.size();

    // Serialize the digital signature.
    memcpy(message.data() + processedBytes, digitalSignature.data(), digitalSignature.size());

    return message;
}

void EndHandshake::deserialize(const std::vector<unsigned char> &message) {
    size_t processedBytes = 0;

    // Check if the type matches the expected one.
    uint8_t receivedType;
    checkIfEnoughSpace(message, processedBytes, sizeof(receivedType));
    memcpy(&receivedType, message.data(), sizeof(receivedType));
    processedBytes += sizeof(receivedType);

    if (receivedType != END_HANDSHAKE) {
        throw SerializationException("Malformed message");
    }

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

std::ostream& operator<<(std::ostream &ostream, const fourinarow::EndHandshake &endHandshake) {
    ostream << "EndHandshake{" << std::endl;
    ostream << "type=" << fourinarow::convertMessageType(endHandshake.getType()) << ',' << std::endl;
    ostream << "publicKey=" << std::endl << fourinarow::dumpVector(endHandshake.getPublicKey());
    ostream << "digitalSignature=" << std::endl << fourinarow::dumpVector(endHandshake.getDigitalSignature());
    ostream << '}';
    return ostream;
}
