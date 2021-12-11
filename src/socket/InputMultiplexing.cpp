#include <errno.h>
#include <string.h>
#include <climits>
#include <SocketException.h>
#include "InputMultiplexing.h"

namespace fourinarow {

InputMultiplexing::InputMultiplexing() {
    FD_ZERO(&masterSet);
    FD_ZERO(&readSet);
    maxDescriptor = 0u;
    numberOfDescriptors = 0u;
}

void InputMultiplexing::addDescriptor(unsigned int descriptor) {
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

void InputMultiplexing::removeDescriptor(unsigned int descriptor) {
    if (descriptor >= FD_SETSIZE) {
        throw SocketException("Invalid descriptor");
    }

    if (FD_ISSET(descriptor, &masterSet)) {
        FD_CLR(descriptor, &masterSet);
        numberOfDescriptors--;
    }
}

bool InputMultiplexing::isReady(unsigned int descriptor) {
    if (descriptor >= FD_SETSIZE) {
        throw SocketException("Invalid descriptor");
    }

    return FD_ISSET(descriptor, &readSet);
}

char* InputMultiplexing::parseError() {
    return strerror(errno);
}

void InputMultiplexing::select() {
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
