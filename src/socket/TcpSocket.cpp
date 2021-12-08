#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <iostream>
#include <SocketException.h>
#include <Constants.h>
#include "TcpSocket.h"

namespace fourinarow {

char *TcpSocket::parseError() {
    return strerror(errno);
}

TcpSocket::TcpSocket(int descriptor, const sockaddr_in &rawAddress) : descriptor(descriptor), rawAddress(rawAddress) {
    char addressBuffer[INET_ADDRSTRLEN];

    /*
     * This constructor is called only after a successful accept(), so there is no need
     * to check the result of inet_ntop(): "descriptor" and "rawAddress" always represent
     * a correctly opened socket.
     */
    inet_ntop(AF_INET, &(this->rawAddress.sin_addr), addressBuffer, INET_ADDRSTRLEN);

    this->address = std::string(addressBuffer);
    this->port = ntohs(this->rawAddress.sin_port);
}

TcpSocket::TcpSocket(std::string address, unsigned short port) : address(std::move(address)), port(port) {
    if (this->address.empty()) {
        throw SocketException("Invalid network address");
    }

    memset(&rawAddress, 0, sizeof(rawAddress));
    rawAddress.sin_family = AF_INET;
    rawAddress.sin_port = htons(this->port);

    auto success = inet_pton(AF_INET, this->address.data(), &rawAddress.sin_addr);
    if (success == 0) {
        throw SocketException("Invalid network address");
    }

    /*
     * The socket descriptor must be initialized at the end, otherwise throwing a SocketException results
     * in a socket not properly closed. Indeed, throwing an exception prevents the constructor to correctly
     * end, so the object will not be fully constructed and the destructor, which contains a call to close(),
     * will not be invoked.
     */
    this->descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (descriptor == -1) {
        throw SocketException(parseError());
    }
}

TcpSocket::~TcpSocket() {
    if (descriptor == -1) {
        return;
    }

    auto success = close(descriptor);
    if (success == -1) {
        std::cerr << "Impossible to close the socket: " << parseError() << std::endl;
    }
}

TcpSocket::TcpSocket(TcpSocket&& that) noexcept
: address(std::move(that.address)), port(that.port), descriptor(that.descriptor), rawAddress(that.rawAddress) {
    that.descriptor = -1; // Avoid a call to close() when destructing "that".
}

const std::string& TcpSocket::getAddress() const {
    return address;
}

unsigned short TcpSocket::getPort() const {
    return port;
}

int TcpSocket::getDescriptor() const {
    return descriptor;
}

void TcpSocket::bind() {
    auto success = ::bind(descriptor, (sockaddr*) &rawAddress, sizeof(rawAddress));
    if (success == -1) {
        throw SocketException(parseError());
    }
}

void TcpSocket::listen(size_t backlog) {
    auto success = ::listen(descriptor, backlog);
    if (success == -1) {
        throw SocketException(parseError());
    }
}

TcpSocket TcpSocket::accept() {
    sockaddr_in clientAddress;
    auto length = sizeof(clientAddress);

    auto newSocketDescriptor = ::accept(descriptor, (sockaddr*) &clientAddress, (socklen_t *) &length);
    if (newSocketDescriptor == -1) {
        throw SocketException(parseError());
    }

    return TcpSocket(newSocketDescriptor, clientAddress);
}

void TcpSocket::connect() {
    auto success = ::connect(descriptor, (sockaddr*) &rawAddress, sizeof(rawAddress));
    if (success == -1) {
        throw SocketException(parseError());
    }
}

void TcpSocket::sendAllBytes(const unsigned char *buffer, size_t bufferLength) {
    ssize_t totalBytesSent = 0;

    while ((size_t) totalBytesSent < bufferLength) { // Safe cast, totalBytesSent is always non-negative.
        auto bytesSent = ::send(descriptor, buffer + totalBytesSent, bufferLength - totalBytesSent, 0);
        if (bytesSent == -1) {
            throw SocketException(parseError());
        }
        totalBytesSent += bytesSent;
    }
}

void TcpSocket::send(const std::vector<unsigned char> &message) {
    if (message.empty()) {
        throw SocketException("Empty message");
    }
    if (message.size() > MAX_MSG_SIZE) {
        std::string errorMessage("Message size is too big. Message size: ");
        errorMessage.append(std::to_string(message.size()));
        errorMessage.append(". Max size: ");
        errorMessage.append(std::to_string(MAX_MSG_SIZE));
        throw SocketException(errorMessage);
    }

    // Send the length of the message on 16 bits.
    uint16_t msgLength = htons(message.size());
    unsigned char msgLengthBytes[sizeof(msgLength)];
    memcpy(msgLengthBytes, &msgLength, sizeof(msgLength));
    sendAllBytes(msgLengthBytes, sizeof(msgLength));

    // Send the entire message.
    sendAllBytes(message.data(), message.size());
}

void TcpSocket::receiveAllBytes(unsigned char *buffer, size_t numberOfBytes) {
    ssize_t totalBytesReceived = 0;

    while ((size_t) totalBytesReceived < numberOfBytes) { // Safe cast, totalBytesReceived is always non-negative.
        auto bytesReceived = ::recv(descriptor, buffer + totalBytesReceived, numberOfBytes - totalBytesReceived, 0);
        if (bytesReceived == -1) {
            throw SocketException(parseError());
        }
        if (bytesReceived == 0) {
            throw SocketException("Remote socket has been closed");
        }
        totalBytesReceived += bytesReceived;
    }
}

std::vector<unsigned char> TcpSocket::receive() {
    // Receive the length of the message on 16 bits.
    uint16_t msgLength;
    unsigned char msgLengthBytes[sizeof(msgLength)];
    receiveAllBytes(msgLengthBytes, sizeof(msgLength));
    memcpy(&msgLength, msgLengthBytes, sizeof(msgLength));

    // Receive the entire message.
    size_t msgSize = ntohs(msgLength);
    if (msgSize == 0) {
        throw SocketException("Empty message");
    }

    std::vector<unsigned char> message(msgSize);
    receiveAllBytes(message.data(), msgSize);
    return message;
}

}

std::ostream& operator<<(std::ostream &ostream, const fourinarow::TcpSocket &socket) {
    ostream << "TcpSocket{";
    ostream << "descriptor=" << socket.getDescriptor();
    ostream << ", address=" << socket.getAddress() << ", port=" << socket.getPort();
    ostream << '}';
    return ostream;
}
