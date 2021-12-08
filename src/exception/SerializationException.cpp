#include "SerializationException.h"

namespace fourinarow {

SerializationException::SerializationException(const std::string &message) : std::runtime_error(message) {}

}
