#include <string.h>
#include <ostream>
#include <Utils.h>
#include <SerializationException.h>
#include "InfoMessage.h"

namespace fourinarow {

InfoMessage::InfoMessage(uint8_t type) : type(type) {}

uint8_t InfoMessage::getType() const {
    return type;
}

std::vector<unsigned char> InfoMessage::serialize() const {
    std::vector<unsigned char> message(sizeof(type));
    memcpy(message.data(), &type, sizeof(type));
    return message;
}

void InfoMessage::deserialize(const std::vector<unsigned char> &message) {
    if (message.size() != sizeof(type)) {
        throw SerializationException("Malformed message");
    }
    memcpy(&type, message.data(), sizeof(type));
}

}

std::ostream& operator<<(std::ostream &ostream, const fourinarow::InfoMessage &infoMessage) {
    ostream << "InfoMessage{";
    ostream << "type=" << fourinarow::convertMessageType(infoMessage.getType());
    ostream << '}';
    return ostream;
}
