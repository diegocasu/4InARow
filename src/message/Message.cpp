#include <SerializationException.h>
#include "Message.h"

namespace fourinarow {

void Message::checkIfEnoughSpace(const std::vector<unsigned char> &message,
                                 size_t processedBytes,
                                 size_t bytesToProcess) {
    /*
     * This check should not be necessary, since size_t is at least on 16 bits and
     * the messages are composed of at most 65535 bytes (cannot send more using TcpSocket).
     */
    if (processedBytes > SIZE_MAX - bytesToProcess) {
        throw SerializationException("Malformed message");
    }

    if (message.size() < processedBytes + bytesToProcess) {
        throw SerializationException("Malformed message");
    }
}

Message::~Message() = default;

}
