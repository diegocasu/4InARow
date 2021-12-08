#ifndef INC_4INAROW_SERIALIZATIONEXCEPTION_H
#define INC_4INAROW_SERIALIZATIONEXCEPTION_H

#include <stdexcept>

namespace fourinarow {

class SerializationException : public std::runtime_error {
    public:
        explicit SerializationException(const std::string &message);
};

}

#endif //INC_4INAROW_SERIALIZATIONEXCEPTION_H
