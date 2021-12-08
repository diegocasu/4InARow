#ifndef INC_4INAROW_TCPSOCKET_H
#define INC_4INAROW_TCPSOCKET_H

#include <arpa/inet.h>
#include <ostream>
#include <vector>

namespace fourinarow {

/**
 * Class representing a socket using IPv4 addresses and exchanging data by means of the TCP protocol.
 * The socket allows to send and receive messages of at most <code>65535</code> bytes.
 * It is up to the user to manage fragmentation and reassembly for messages of bigger sizes.
 */
class TcpSocket {
    private:
        std::string address;
        unsigned short port;
        int descriptor;
        sockaddr_in rawAddress;

        /**
         * Creates a TCP socket representing a socket already connected at system level.
         * This constructor is useful to represent a socket created by a successful <code>accept()</code>.
         * @param descriptor  the descriptor of the socket.
         * @param rawAddress  the structure representing the address to which the socket is connected.
         */
        TcpSocket(int descriptor, const sockaddr_in &rawAddress);

        /**
         * Returns a string containing a human readable description of the error
         * that occurred while operating on the socket.
         * @return  the string containing a readable description of the error.
         */
        char* parseError();

        /**
         * Sends all the bytes composing a binary message through a connected socket.
         * @param buffer        the buffer containing the binary message.
         * @param bufferLength  the length of the buffer.
         * @throws SocketException  if the send operation fails.
         */
        void sendAllBytes(const unsigned char *buffer, size_t bufferLength);

        /**
         * Retrieves all the bytes composing a binary message from a connected socket.
         * @param buffer         the buffer that will hold the message.
         * @param numberOfBytes  the number of bytes composing the message.
         * @throws SocketException  if the receive operation fails, or the remote socket has been closed.
         */
        void receiveAllBytes(unsigned char *buffer, size_t numberOfBytes);
    public:
        /**
         * Creates a TCP socket, either to connect to a remote server or to bind to a local address.
         * The role of the socket (client/server) is not determined by the constructor,
         * but by the operations invoked on the object.
         * @param address  the IPv4 address to which the socket will bind or connect.
         * @param port     the port to which the socket will bind or connect.
         * @throws SocketException  if the given address is invalid, or the system socket cannot be created.
         */
        TcpSocket(std::string address, unsigned short port);

        /**
         * Destroys a TCP socket, automatically closing the underlying system socket.
         */
        ~TcpSocket();

        /**
         * Move constructs a TCP socket, automatically transferring the ownership of
         * the system socket, so that the moved object cannot access it anymore.
         * @param that  the socket to move.
         */
        TcpSocket(TcpSocket && that) noexcept;

        TcpSocket(const TcpSocket&) = delete;
        TcpSocket& operator=(const TcpSocket&) = delete;
        TcpSocket& operator=(TcpSocket&&) = delete;

        const std::string& getAddress() const;
        unsigned short getPort() const;
        int getDescriptor() const;

        /**
         * Binds the socket to the network address and port specified at construction time.
         * @throws SocketException  if the bind operation fails.
         */
        void bind();

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
         * Connects the socket to the remote address specified at construction time.
         * @throws SocketException  if the connection to the remote address fails.
         */
        void connect();

        /**
         * Sends a binary message through a connected socket. The method is blocking:
         * the socket waits until the entire message has been sent. A message can be
         * composed of at most <code>65535</code> bytes.
         * @param message  the binary message to send.
         * @throws SocketException  if the message is empty, exceeds the maximum size,
         *                          or an error occurred while performing the send.
         */
        void send(const std::vector<unsigned char> &message);

        /**
         * Retrieves a binary message from a connected socket. The method is blocking:
         * the socket waits until the entire message sent by the other end has been
         * received. A received message is composed of at most <code>65535</code> bytes.
         * @return  a binary message.
         * @throws SocketException  if the message is empty, the remote socket has been closed,
         *                          or an error occurred while performing the receive.
         */
        std::vector<unsigned char> receive();
};

}

std::ostream& operator<<(std::ostream &ostream, const fourinarow::TcpSocket &socket);

#endif //INC_4INAROW_TCPSOCKET_H
