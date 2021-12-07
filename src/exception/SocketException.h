#ifndef INC_4INAROW_SOCKETEXCEPTION_H
#define INC_4INAROW_SOCKETEXCEPTION_H

#include <stdexcept>

namespace fourinarow {

class SocketException : public std::runtime_error {
    public:
        explicit SocketException(const std::string &message);
};

}

#endif //INC_4INAROW_SOCKETEXCEPTION_H
