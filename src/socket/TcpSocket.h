#ifndef INC_4INAROW_TCPSOCKET_H
#define INC_4INAROW_TCPSOCKET_H

#include <arpa/inet.h>
#include <ostream>
#include <string>
#include <vector>

namespace fourinarow {

/**
 * Class representing a socket using IPv4 addresses and exchanging data by means of the TCP protocol.
 * The socket allows to send and receive messages of at most <code>65535</code> bytes.
 * It is up to the user to manage fragmentation and reassembly for messages of bigger sizes.
 */
class TcpSocket {
    private:
        std::string sourceAddress;
        unsigned short sourcePort;
        sockaddr_in rawSourceAddress;
        std::string destinationAddress;
        unsigned short destinationPort;
        sockaddr_in rawDestinationAddress;
        int descriptor;

        /**
         * Creates a TCP socket representing a socket already connected at system level.
         * This constructor is useful to represent a socket created by a successful <code>accept()</code>.
         * @param descriptor             the descriptor of the socket.
         * @param rawDestinationAddress  the structure representing the address to which the socket is connected.
         */
        TcpSocket(int descriptor, const sockaddr_in &rawDestinationAddress);

        /**
         * Returns a string containing a human readable description of the error
         * that occurred while operating on the socket.
         * @return  the string containing a readable description of the error.
         */
        char* parseError() const;

        /**
         * Sends all the bytes composing a binary message through a connected socket.
         * @param buffer        the buffer containing the binary message.
         * @param bufferLength  the length of the buffer.
         * @throws SocketException  if the send operation fails.
         */
        void sendAllBytes(const unsigned char *buffer, size_t bufferLength) const;

        /**
         * Retrieves all the bytes composing a binary message from a connected socket.
         * @param buffer         the buffer that will hold the message.
         * @param numberOfBytes  the number of bytes composing the message.
         * @throws SocketException  if the receive operation fails, or the remote socket has been closed.
         */
        void receiveAllBytes(unsigned char *buffer, size_t numberOfBytes) const;
    public:
        /**
         * Creates a TCP socket using IPv4 addresses. The method requests only the creation
         * of a system socket: the address to which bind or connect is specified
         * with the dedicated methods.
         * @throws SocketException  if the system socket cannot be created.
         */
        TcpSocket();

        /**
         * Destroys a TCP socket, automatically closing the underlying system socket.
         */
        ~TcpSocket();

        /**
         * Move constructs a TCP socket, automatically transferring the ownership of
         * the system socket, so that the moved object cannot access it anymore.
         * Calling <code>bind(), listen(), accept(), connect(), send()</code> or
         * <code>receive()</code> on a moved socket fails and
         * results in a <code>SocketException</code>.
         * @param that  the socket to move.
         */
        TcpSocket(TcpSocket &&that) noexcept;

        /**
         * Move assigns a TCP socket, automatically transferring the ownership of
         * the system socket, so that the moved object cannot access it anymore.
         * Calling <code>bind(), listen(), accept(), connect(), send()</code> or
         * <code>receive()</code> on a moved socket fails and
         * results in a <code>SocketException</code>.
         * @param that  the socket to move.
         */
        TcpSocket& operator=(TcpSocket &&that) noexcept;

        TcpSocket(const TcpSocket&) = delete;
        TcpSocket& operator=(const TcpSocket&) = delete;

        const std::string &getSourceAddress() const;
        unsigned short getSourcePort() const;
        const std::string &getDestinationAddress() const;
        unsigned short getDestinationPort() const;
        int getDescriptor() const;

        /**
         * Returns the source address in the form ADDRESS:PORT.
         * @return  the full source address.
         */
        const std::string getFullSourceAddress() const;

        /**
         * Returns the destination address in the form ADDRESS:PORT.
         * @return  the full destination address.
         */
        const std::string getFullDestinationAddress() const;

        /**
         * Binds the socket to the specified address.
         * @param address  the IPv4 address.
         * @param port     the port.
         * @throws SocketException  if the given address is invalid, or the bind operation fails.
         */
        void bind(std::string address, unsigned short port);

        /**
         * Marks the socket as passive, i.e. able to receive incoming connection requests.
         * @param backlog  the dimension of the backlog queue for requests.
         * @throws SocketException  if the listening operation fails.
         */
        void listen(size_t backlog);

        /**
         * Accepts an incoming connection request. The method is blocking: the socket
         * waits until an incoming request arrives.
         * @return  the socket representing a new connection with a client.
         * @throws SocketException  if the connection cannot be accepted.
         */
        TcpSocket accept();

        /**
         * Connects the socket to the specified remote address. The method is blocking:
         * the socket waits until the connection request is not accepted.
         * @param address  the IPv4 address.
         * @param port     the port.
         * @throws SocketException  if the given address is invalid, or
         *                          the connection to the remote address fails.
         */
        void connect(std::string address, unsigned short port);

        /**
         * Sends a binary message through a connected socket. The method is blocking:
         * the socket waits until the entire message has been sent. A message can be
         * composed of at most <code>65535</code> bytes.
         * @param message  the binary message to send.
         * @throws SocketException  if the message is empty, exceeds the maximum size,
         *                          or an error occurred while performing the send.
         */
        void send(const std::vector<unsigned char> &message) const;

        /**
         * Retrieves a binary message from a connected socket. The method is blocking:
         * the socket waits until the entire message sent by the other end has been
         * received. A received message is composed of at most <code>65535</code> bytes.
         * @return  a binary message.
         * @throws SocketException  if the message is empty, the remote socket has been closed,
         *                          or an error occurred while performing the receive.
         */
        std::vector<unsigned char> receive() const;

        bool operator==(const TcpSocket &rhs) const;
        bool operator!=(const TcpSocket &rhs) const;
};

}

std::ostream& operator<<(std::ostream &ostream, const fourinarow::TcpSocket &socket);

#endif //INC_4INAROW_TCPSOCKET_H
