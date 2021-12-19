#include <errno.h>
#include <string.h>
#include <SocketException.h>
#include "InputMultiplexer.h"

namespace fourinarow {

InputMultiplexer::InputMultiplexer() {
    FD_ZERO(&masterSet);
    FD_ZERO(&readSet);
    maxDescriptor = 0u;
    numberOfDescriptors = 0u;
}

void InputMultiplexer::addDescriptor(unsigned int descriptor) {
    if (descriptor >= FD_SETSIZE) {
        throw SocketException("Invalid descriptor");
    }

    if (numberOfDescriptors == FD_SETSIZE) {
        throw SocketException("Cannot monitor more than " + std::to_string(FD_SETSIZE) + " sockets at a time");
    }

    FD_SET(descriptor, &masterSet);
    numberOfDescriptors++;

    if (descriptor > maxDescriptor)
        maxDescriptor = descriptor;
}

void InputMultiplexer::removeDescriptor(unsigned int descriptor) {
    if (descriptor >= FD_SETSIZE) {
        throw SocketException("Invalid descriptor");
    }

    if (FD_ISSET(descriptor, &masterSet)) {
        FD_CLR(descriptor, &masterSet);
        numberOfDescriptors--;
    }
}

bool InputMultiplexer::isReady(unsigned int descriptor) const {
    if (descriptor >= FD_SETSIZE) {
        throw SocketException("Invalid descriptor");
    }

    return FD_ISSET(descriptor, &readSet);
}

char* InputMultiplexer::parseError() const {
    return strerror(errno);
}

void InputMultiplexer::select() {
    if (numberOfDescriptors == 0) {
        return;
    }

    readSet = masterSet;
    auto success = ::select(maxDescriptor + 1, &readSet, nullptr, nullptr, nullptr);

    if (success == -1) {
        throw SocketException(parseError());
    }
}

}
