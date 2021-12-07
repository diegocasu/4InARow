#include "CryptoException.h"

namespace fourinarow {

CryptoException::CryptoException(const std::string &message) : std::runtime_error(message) {}

}
