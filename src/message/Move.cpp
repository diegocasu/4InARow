#include <string.h>
#include <SerializationException.h>
#include <Utils.h>
#include "Move.h"

namespace fourinarow {

Move::Move(uint8_t column) : column(column) {}

Move::~Move() {
    cleanse(type);
    cleanse(column);
}

uint8_t fourinarow::Move::getType() const {
    return type;
}

uint8_t fourinarow::Move::getColumn() const {
    return column;
}

std::vector<unsigned char> Move::serialize() const {
    checkColumnIndexValidity<SerializationException>(column);

    size_t processedBytes = 0;
    size_t outputSize = sizeof(type) + sizeof(COLUMNS);
    std::vector<unsigned char> message(outputSize);

    // Serialize the type.
    memcpy(message.data(), &type, sizeof(type));
    processedBytes += sizeof(type);

    // Serialize the column index.
    memcpy(message.data() + processedBytes, &column, sizeof(column));

    return message;
}

void Move::deserialize(const std::vector<unsigned char> &message) {
    size_t processedBytes = 0;

    // Check if the type matches the expected one.
    uint8_t receivedType;
    checkIfEnoughSpace(message, processedBytes, sizeof(receivedType));
    memcpy(&receivedType, message.data(), sizeof(receivedType));
    processedBytes += sizeof(receivedType);

    if (receivedType != MOVE) {
        throw SerializationException("Malformed message");
    }

    // Deserialize the column index and check if valid.
    checkIfEnoughSpace(message, processedBytes, sizeof(column));
    memcpy(&column, message.data() + processedBytes, sizeof(column));
    checkColumnIndexValidity<SerializationException>(column);
}

}

std::ostream& operator<<(std::ostream &ostream, const fourinarow::Move &move) {
    ostream << "Move{";
    ostream << "type=" << fourinarow::convertMessageType(move.getType()) << ", ";
    ostream << "column=" << unsigned(move.getColumn());
    ostream << '}';
    return ostream;
}