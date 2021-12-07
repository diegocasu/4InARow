#include "SocketException.h"

namespace fourinarow {

SocketException::SocketException(const std::string &message) : std::runtime_error(message) {}

}
