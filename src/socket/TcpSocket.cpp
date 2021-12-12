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

char *TcpSocket::parseError() const {
    return strerror(errno);
}

TcpSocket::TcpSocket(int descriptor, const sockaddr_in &rawDestinationAddress)
: sourceAddress("unspecified"), sourcePort(0), rawDestinationAddress(rawDestinationAddress), descriptor(descriptor) {
    char addressBuffer[INET_ADDRSTRLEN];

    /*
     * This constructor is called only after a successful accept(), so there is no need
     * to check the result of inet_ntop(): "descriptor" and "rawDestinationAddress" always represent
     * a correctly opened socket.
     */
    inet_ntop(AF_INET, &(this->rawDestinationAddress.sin_addr), addressBuffer, INET_ADDRSTRLEN);

    this->destinationAddress = std::string(addressBuffer);
    this->destinationPort = ntohs(this->rawDestinationAddress.sin_port);
}

TcpSocket::TcpSocket()
: sourceAddress("unspecified"), sourcePort(0), destinationAddress("unspecified"), destinationPort(0) {
    descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (descriptor == -1) {
        throw SocketException(parseError());
    }
}

TcpSocket::~TcpSocket() {
    if (descriptor != -1) {
        auto success = close(descriptor);
        if (success == -1) {
            std::cerr << "Impossible to close the socket: " << parseError() << std::endl;
        }
    }
}

TcpSocket::TcpSocket(TcpSocket &&that) noexcept
    : sourceAddress(std::move(that.sourceAddress)),
      sourcePort(that.sourcePort),
      rawSourceAddress(that.rawSourceAddress),
      destinationAddress(std::move(that.destinationAddress)),
      destinationPort(that.destinationPort),
      rawDestinationAddress(that.rawDestinationAddress),
      descriptor(that.descriptor) {
    that.sourceAddress = "unspecified";
    that.destinationAddress = "unspecified";
    that.descriptor = -1; // Avoid a call to close() when destructing "that".
}

TcpSocket& TcpSocket::operator=(TcpSocket &&that) noexcept {
    if (descriptor != -1) {
        auto success = close(descriptor);
        if (success == -1) {
            std::cerr << "Impossible to close the socket: " << parseError() << std::endl;
        }
    }

    sourceAddress = std::move(that.sourceAddress);
    sourcePort = that.sourcePort;
    rawSourceAddress = that.rawSourceAddress;
    destinationAddress = std::move(that.destinationAddress);
    destinationPort = that.destinationPort;
    rawDestinationAddress = that.rawDestinationAddress;
    descriptor = that.descriptor;

    that.sourceAddress = "unspecified";
    that.destinationAddress = "unspecified";
    that.descriptor = -1; // Avoid a call to close() when destructing "that".

    return *this;
}

const std::string &TcpSocket::getSourceAddress() const {
    return sourceAddress;
}

unsigned short TcpSocket::getSourcePort() const {
    return sourcePort;
}

const std::string &TcpSocket::getDestinationAddress() const {
    return destinationAddress;
}

unsigned short TcpSocket::getDestinationPort() const {
    return destinationPort;
}

int TcpSocket::getDescriptor() const {
    return descriptor;
}

void TcpSocket::bind(std::string address, unsigned short port) {
    sourceAddress = std::move(address);
    sourcePort = port;

    memset(&rawSourceAddress, 0, sizeof(rawSourceAddress));
    rawSourceAddress.sin_family = AF_INET;
    rawSourceAddress.sin_port = htons(sourcePort);

    auto success = inet_pton(AF_INET, sourceAddress.data(), &rawSourceAddress.sin_addr);
    if (success == 0) {
        throw SocketException("Invalid network address");
    }

    success = ::bind(descriptor, (sockaddr*) &rawSourceAddress, sizeof(rawSourceAddress));
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

void TcpSocket::connect(std::string address, unsigned short port) {
    destinationAddress = std::move(address);
    destinationPort = port;

    memset(&rawDestinationAddress, 0, sizeof(rawDestinationAddress));
    rawDestinationAddress.sin_family = AF_INET;
    rawDestinationAddress.sin_port = htons(destinationPort);

    auto success = inet_pton(AF_INET, destinationAddress.data(), &rawDestinationAddress.sin_addr);
    if (success == 0) {
        throw SocketException("Invalid network address");
    }

    success = ::connect(descriptor, (sockaddr*) &rawDestinationAddress, sizeof(rawDestinationAddress));
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
        throw SocketException("The message size is too big. Message size: " +
                              std::to_string(message.size()) +
                              " bytes. Max message size: " +
                              std::to_string(MAX_MSG_SIZE) +
                              " bytes");
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
    ostream << ", sourceAddress=" << socket.getSourceAddress();
    ostream << ", sourcePort=" << socket.getSourcePort();
    ostream << ", destinationAddress=" << socket.getDestinationAddress();
    ostream << ", destinationPort=" << socket.getDestinationPort();
    ostream << '}';
    return ostream;
}
