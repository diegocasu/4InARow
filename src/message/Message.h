#ifndef INC_4INAROW_MESSAGE_H
#define INC_4INAROW_MESSAGE_H

#include <vector>

namespace fourinarow {

/**
 * Base class representing a message.
 */
class Message {
    protected:
        /**
         * Checks if a partially processed message has at least
         * a certain number of bytes left to process.
         * @param message         the message under processing.
         * @param processedBytes  the number of bytes already processed.
         * @param bytesToProcess  the number of bytes left to process.
         * @throws SerializationException  if the message does not contain at least
         *                                 <code>processedBytes + bytesToProcess</code> bytes.
         */
        void checkIfEnoughSpace(const std::vector<unsigned char> &message, size_t processedBytes, size_t bytesToProcess);
    public:
        virtual ~Message() = 0;

        /**
         * Serializes a message to a binary format.
         * @return  the message in binary format.
         * @throws SerializationException  if the message has not the expected format.
         */
        virtual std::vector<unsigned char> serialize() = 0;

        /**
         * Deserializes a message in binary format.
         * @param message  the message in binary format.
         * @throws SerializationException  if the message has not the expected format.
         */
        virtual void deserialize(const std::vector<unsigned char> &message) = 0;
};

}

#endif //INC_4INAROW_MESSAGE_H
