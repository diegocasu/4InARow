#ifndef INC_4INAROW_INPUTMULTIPLEXING_H
#define INC_4INAROW_INPUTMULTIPLEXING_H

#include <sys/types.h>
#include <ostream>

namespace fourinarow {

/**
 * Class representing an input multiplexer for sockets.
 * The multiplexer is able to monitor a set of sockets and detect when at least one of them
 * is ready for a read operation. The maximum number of sockets that can be monitored
 * at the same time is equal to <code>FD_SETSIZE</code>. A socket descriptor is considered valid
 * if and only if its value is in the interval <code>[0, FD_SETSIZE - 1)</code>.
 */
class InputMultiplexing {
    private:
        fd_set masterSet;
        fd_set readSet;
        unsigned int maxDescriptor;
        unsigned int numberOfDescriptors;

        /**
         * Returns a string containing a human readable description of the error
         * that occurred while using <code>select()</code>.
         * @return  the string containing a readable description of the error.
         */
        char* parseError();
    public:
        InputMultiplexing();

        /**
         * Destroys the object, without closing the sockets added to the set of monitored ones.
         * It is up to the caller to close them individually, if needed.
         */
        ~InputMultiplexing() = default;

        InputMultiplexing(const InputMultiplexing&) = default;
        InputMultiplexing(InputMultiplexing&&) = default;
        InputMultiplexing& operator=(const InputMultiplexing&) = default;
        InputMultiplexing& operator=(InputMultiplexing&&) = default;

        /**
         * Adds a socket descriptor to the set of monitored ones. If the descriptor
         * is already in the set, the method has no effect.
         * @param descriptor  the socket descriptor.
         * @throws SocketException  if the descriptor is invalid, or the maximum number of sockets
         *                          that can be monitored at the same time has been reached.
         */
        void addDescriptor(unsigned int descriptor);

        /**
         * Removes a socket descriptor from the set of monitored ones. If the descriptor
         * is not in the set, the method has no effect.
         * @param descriptor  the socket descriptor.
         * @throws SocketException  if the descriptor is invalid.
         */
        void removeDescriptor(unsigned int descriptor);

        /**
         * Checks if the given socket is ready for performing a read, namely if one of
         * the following conditions is true:
         * 1) there is at least one byte to read;
         * 2) the socket has been closed;
         * 3) the socket is in an error state;
         * 4) the socket is a listening one and there are pending connections.
         * @param descriptor  the socket descriptor.
         * @return            true if the socket is ready, false otherwise.
         * @throws SocketException  if the descriptor is invalid.
         */
        bool isReady(unsigned int descriptor);

        /**
         * Waits until at least one of the sockets being monitored is ready.
         * The method is blocking, unless the set of sockets is empty.
         * After the method returns, the caller can test if a
         * socket is ready by calling <code>isReady()</code>.
         * @throws SocketException  if an error occurred while monitoring the sockets.
         */
        void select();
};

}

#endif //INC_4INAROW_INPUTMULTIPLEXING_H
