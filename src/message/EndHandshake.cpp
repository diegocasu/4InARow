#include <string.h>
#include <ostream>
#include <Utils.h>
#include <SerializationException.h>
#include "EndHandshake.h"

namespace fourinarow {

EndHandshake::EndHandshake(std::vector<unsigned char> digitalSignature)
: digitalSignature(std::move(digitalSignature)) {}

uint8_t EndHandshake::getType() const {
    return type;
}

const std::vector<unsigned char>& EndHandshake::getDigitalSignature() const {
    return digitalSignature;
}

void EndHandshake::checkIfSerializable() {
    if (digitalSignature.size() != DIGITAL_SIGNATURE_SIZE) {
        std::string errorMessage("The digital signature size must be exactly ");
        errorMessage.append(std::to_string(DIGITAL_SIGNATURE_SIZE));
        errorMessage.append(" bytes. Digital signature size: ");
        errorMessage.append(std::to_string(digitalSignature.size()));
        errorMessage.append(" bytes");
        throw SerializationException(errorMessage);
    }
}

std::vector<unsigned char> EndHandshake::serialize() {
    checkIfSerializable();

    size_t processedBytes = 0;
    size_t outputSize = sizeof(type) + digitalSignature.size();
    std::vector<unsigned char> message(outputSize);

    // Serialize the type.
    memcpy(message.data(), &type, sizeof(type));
    processedBytes += sizeof(type);

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

    // Deserialize the digital signature.
    checkIfEnoughSpace(message, processedBytes, DIGITAL_SIGNATURE_SIZE);
    digitalSignature.resize(DIGITAL_SIGNATURE_SIZE);
    memcpy(digitalSignature.data(), message.data() + processedBytes, DIGITAL_SIGNATURE_SIZE);
}

}

std::ostream& operator<<(std::ostream &ostream, const fourinarow::EndHandshake &endHandshake) {
    ostream << "EndHandshake{" << std::endl;
    ostream << "type=" << fourinarow::convertMessageType(endHandshake.getType()) << ',' << std::endl;
    ostream << "digitalSignature=" << std::endl << fourinarow::dumpVector(endHandshake.getDigitalSignature());
    ostream << '}';
    return ostream;
}
