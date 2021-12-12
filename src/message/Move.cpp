#include <string.h>
#include <SerializationException.h>
#include <Utils.h>
#include "Move.h"

namespace fourinarow {

Move::Move(uint8_t row, uint8_t column) : row(row), column(column) {}

Move::~Move() {
    cleanse(type);
    cleanse(row);
    cleanse(column);
}

uint8_t fourinarow::Move::getType() const {
    return type;
}

uint8_t fourinarow::Move::getRow() const {
    return row;
}

uint8_t fourinarow::Move::getColumn() const {
    return column;
}

bool Move::isRowValid(uint8_t candidateRow) const {
    return candidateRow < ROWS;
}

bool Move::isColumnValid(uint8_t candidateColumn) const {
    return candidateColumn < COLUMNS;
}

void Move::checkIfSerializable() const {
    if (!isRowValid(row)) {
        throw SerializationException("The row must be a number between 0 and " +
                                     std::to_string(ROWS - 1) +
                                     ". Row: " +
                                     std::to_string(unsigned(row)));
    }

    if (!isColumnValid(column)) {
        throw SerializationException("The column must be a number between 0 and " +
                                     std::to_string(COLUMNS - 1) +
                                     ". Column: " +
                                     std::to_string(unsigned(column)));
    }
}

std::vector<unsigned char> Move::serialize() const {
    checkIfSerializable();

    size_t processedBytes = 0;
    size_t outputSize = sizeof(type) + sizeof(ROWS) + sizeof(COLUMNS);
    std::vector<unsigned char> message(outputSize);

    // Serialize the type.
    memcpy(message.data(), &type, sizeof(type));
    processedBytes += sizeof(type);

    // Serialize the row index.
    memcpy(message.data() + processedBytes, &row, sizeof(row));
    processedBytes += sizeof(row);

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

    // Deserialize the row index and check if valid.
    uint8_t receivedRow;
    checkIfEnoughSpace(message, processedBytes, sizeof(receivedRow));
    memcpy(&receivedRow, message.data() + processedBytes, sizeof(receivedRow));
    processedBytes += sizeof(receivedRow);

    if (!isRowValid(receivedRow)) {
        throw SerializationException("Malformed message");
    }
    row = receivedRow;

    // Deserialize the column index and check if valid.
    uint8_t receivedColumn;
    checkIfEnoughSpace(message, processedBytes, sizeof(receivedColumn));
    memcpy(&receivedColumn, message.data() + processedBytes, sizeof(receivedColumn));

    if (!isColumnValid(receivedColumn)) {
        throw SerializationException("Malformed message");
    }
    column = receivedColumn;
}

}

std::ostream& operator<<(std::ostream &ostream, const fourinarow::Move &move) {
    ostream << "Move{";
    ostream << "type=" << fourinarow::convertMessageType(move.getType()) << ", ";
    ostream << "row=" << unsigned(move.getRow()) << ", ";
    ostream << "column=" << unsigned(move.getColumn());
    ostream << '}';
    return ostream;
}