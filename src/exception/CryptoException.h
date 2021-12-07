#ifndef INC_4INAROW_CRYPTOEXCEPTION_H
#define INC_4INAROW_CRYPTOEXCEPTION_H

#include <stdexcept>

namespace fourinarow {

class CryptoException : public std::runtime_error {
    public:
        explicit CryptoException(const std::string &message);
};

}

#endif //INC_4INAROW_CRYPTOEXCEPTION_H
