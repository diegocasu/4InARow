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
 * It is up to the user to manage fragmentation and reassembly for messages of bigger size.
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
         * This constructor is useful to represent a socket created by
         * a successful <code>accept()</code>.
         * @param descriptor             the descriptor of the socket.
         * @param rawDestinationAddress  the structure representing the address
         *                               to which the socket is connected.
         */
        TcpSocket(int descriptor, const sockaddr_in &rawDestinationAddress);

        /**
         * Returns a string containing a human readable description of the error
         * that occurred while operating on the socket.
         * @return  the string containing the error.
         */
        char* parseError() const;

        /**
         * Sends all the bytes of a message through a connected socket.
         * @param buffer        the buffer containing the message.
         * @param bufferLength  the length of the buffer.
         * @throws SocketException  if the operation fails.
         */
        void sendAllBytes(const unsigned char *buffer, size_t bufferLength) const;

        /**
         * Receives all the bytes of a message from a connected socket.
         * @param buffer         the buffer that will hold the message.
         * @param numberOfBytes  the number of bytes composing the message.
         * @throws SocketException  if the operation fails, or the remote socket has been closed.
         */
        void receiveAllBytes(unsigned char *buffer, size_t numberOfBytes) const;
    public:
        /**
         * Creates a TCP socket using IPv4 addresses.
         * The method requests only the creation of a system socket.
         * @throws SocketException  if the socket cannot be created.
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

        const std::string& getSourceAddress() const;
        unsigned short getSourcePort() const;
        const std::string& getDestinationAddress() const;
        unsigned short getDestinationPort() const;
        int getDescriptor() const;

        /**
         * Returns the source address in the form <code>ADDRESS:PORT</code>.
         * @return  the full source address.
         */
        const std::string getFullSourceAddress() const;

        /**
         * Returns the destination address in the form <code>ADDRESS:PORT</code>.
         * @return  the full destination address.
         */
        const std::string getFullDestinationAddress() const;

        /**
         * Binds the socket to the specified address.
         * @param address  the IPv4 address.
         * @param port     the port.
         * @throws SocketException  if the given address is invalid,
         *                          or the bind operation fails.
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
         * @return  the socket representing the new connection.
         * @throws SocketException  if the connection cannot be accepted.
         */
        TcpSocket accept();

        /**
         * Connects the socket to the specified remote address. The method is blocking:
         * the socket waits until the connection request is accepted.
         * @param address  the IPv4 address.
         * @param port     the port.
         * @throws SocketException  if the given address is invalid,
         *                          or the connection to the remote address fails.
         */
        void connect(std::string address, unsigned short port);

        /**
         * Sends a binary message through a connected socket. The method is blocking:
         * the socket waits until the entire message has been sent. A message can be
         * composed of at most <code>65535</code> bytes.
         * @param message  the binary message to send.
         * @throws SocketException  if the message is empty or exceeds the maximum size,
         *                          or an error occurs while performing the send.
         */
        void send(const std::vector<unsigned char> &message) const;

        /**
         * Receives a binary message from a connected socket. The method is blocking:
         * the socket waits until the entire message has been received.
         * A received message is composed of at most <code>65535</code> bytes.
         * @return  a binary message.
         * @throws SocketException  if the message is empty,
         *                          or an error occurs while performing the receive,
         *                          or the remote socket has been closed.
         */
        std::vector<unsigned char> receive() const;

        /**
         * Receives a binary message from a connected socket. The method is non-blocking:
         * if no bytes have been received and the given number of seconds has passed,
         * an exception is thrown. If the given number of seconds is equal to <code>0</code>,
         * the method becomes blocking, i.e. it acts as <code>receive()</code>.
         * A received message is composed of at most <code>65535</code> bytes.
         * @param seconds  the timeout expressed in seconds.
         * @return         a binary message.
         * @throws SocketException  if the message is empty,
         *                          or an error occurs while performing the receive,
         *                          or the timeout expires,
         *                          or the remote socket has been closed.
         */
        std::vector<unsigned char> receiveWithTimeout(unsigned long seconds) const;

        bool operator==(const TcpSocket &rhs) const;
        bool operator!=(const TcpSocket &rhs) const;
};

}

std::ostream& operator<<(std::ostream &ostream, const fourinarow::TcpSocket &socket);

#endif //INC_4INAROW_TCPSOCKET_H
