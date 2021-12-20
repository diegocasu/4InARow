#ifndef INC_4INAROW_HANDSHAKEHANDLER_H
#define INC_4INAROW_HANDSHAKEHANDLER_H

#include "Handler.h"
#include <CertificateStore.h>
#include <PlayerMessage.h>

namespace fourinarow {

/**
 * Class representing a handler for the handshake with the server
 * or with another client.
 */
class HandshakeHandler : public Handler {
    private:
        using P2PHandshakeResult = std::tuple<std::unique_ptr<TcpSocket>, std::unique_ptr<Player>, bool>;

        /**
         * Sends a <code>CLIENT_HELLO</code> message to the server, starting the handshake.
         * @param socket           the socket used to communicate with the server.
         * @param myselfForServer  the object storing the quantities needed for the handshake.
         * @param username         the username of the client.
         * @throws runtime_error  if an error occurred while sending the <code>CLIENT_HELLO</code> message.
         */
        static void sendClientHello(const TcpSocket &socket, Player &myselfForServer, const std::string &username);

        /**
         * Checks if the certificate of the server is valid.
         * @param certificateStore   the store of trusted certificates.
         * @param serverCertificate  the certificate of the server.
         * @return                   true if the certificate is valid, false otherwise.
         */
        static bool isValidCertificate(const CertificateStore &certificateStore, const Certificate &serverCertificate);

        /**
         * Waits for a <code>SERVER_HELLO</code> message sent by the server.
         * @param socket            the socket used to communicate with the server.
         * @param myselfForServer   the object storing the quantities needed for the handshake.
         * @param certificateStore  the certificate store used to verify untrusted certificates.
         * @throws runtime_error  if an error occurred while receiving the <code>SERVER_HELLO</code> message,
         *                        or the server certificate is invalid, or the digital signature of the
         *                        freshness proof is invalid.
         */
        static void receiveServerHello(const TcpSocket &socket,
                                       Player &myselfForServer,
                                       const CertificateStore &certificateStore);

        /**
         * Ends the handshake with the server by sending an <code>END_HANDSHAKE</code> message.
         * Then, it derives the symmetric session key and receives the first <code>PLAYER_LIST</code> message.
         * @param socket            the socket used to communicate with the server.
         * @param myselfForServer   the object storing the quantities needed for the handshake.
         * @param digitalSignature  the digital signature tool.
         * @return                  the first player list.
         * @throws runtime_error  if an error occurred while sending the <code>END_HANDSHAKE</code> message,
         *                        or deriving the session key, or receiving the <code>PLAYER_LIST</code> message.
         */
        static std::string endHandshake(const TcpSocket &socket,
                                        Player &myselfForServer,
                                        const DigitalSignature &digitalSignature);
        /**
         * Attempts to connect to the other player, which is acting as server
         * in the P2P handshake. If the connection fails, another attempt is performed
         * after 1 second, until the maximum number of retries specified by
         * <code>P2P_MAX_CONNECTION_RETRIES</code> is reached.
         * @param socket              the socket used to communicate with the other player.
         * @param otherPlayerAddress  the IPv4 address of the other player.
         * @throws SocketException  if the connection to the remote player has failed.
         */
        static void connectToPlayer(TcpSocket &socket, const std::string &otherPlayerAddress);

        /**
         * Waits for a connection attempt done by the other player, which is acting as client
         * in the P2P handshake. If the connection fails, another attempt is performed
         * after 1 second, until the maximum number of retries specified by
         * <code>P2P_MAX_CONNECTION_RETRIES</code> is reached.
         * @param socket              the socket used to communicate with the other player.
         * @param otherPlayerAddress  the IPv4 address of the other player.
         * @return                    the socket used to communicate with the player.
         * @throws SocketException  if the connection with the remote player failed.
         */
        static TcpSocket waitForPlayerConnection(TcpSocket &socket, const std::string &otherPlayerAddress);

        /**
         * Sends a <code>PLAYER1_HELLO</code> message to the other player, starting the handshake.
         * @param socket             the socket used to communicate with the player.
         * @param myselfForOpponent  the object storing the quantities needed for the handshake.
         * @throws runtime_error  if an error occurred while sending the message.
         */
        static void sendPlayer1Hello(const TcpSocket &socket, Player &myselfForOpponent);

        /**
         * Handles the reception of a <code>PLAYER1_HELLO</code> message, responding
         * with a <code>PLAYER2_HELLO</code> message.
         * @param socket            the socket used to communicate with the player.
         * @param opponent          the object storing the quantities needed for the handshake.
         * @param digitalSignature  the digital signature tool.
         * @throws runtime_error    if an error occurred while receiving the message or responding.
         * @throws SocketException  if an error occurred while sending an error message.
         */
        static void handlePlayer1Hello(const TcpSocket &socket,
                                       Player &opponent,
                                       const DigitalSignature &digitalSignature);

        /**
         * Waits for a <code>PLAYER2_HELLO</code> message sent by the other player.
         * @param socket             the socket used to communicate with the player.
         * @param myselfForOpponent  the object storing the quantities needed for the handshake.
         * @param playerMessage      the <code>PLAYER</code> message sent by the server,
         *                           containing the public key used to verify the digital signature
         *                           of the freshness proof.
         * @throws runtime_error  if an error occurred while receiving the message,
         *                        or the digital signature of the freshness proof is invalid.
         */
        static void receivePlayer2Hello(const TcpSocket &socket,
                                        Player &myselfForOpponent,
                                        const PlayerMessage &playerMessage);

        /**
         * Handles the reception of a <code>END_HANDSHAKE</code> message.
         * If the handshake ends correctly, it derives the symmetric session keys.
         * @param socket         the socket used to communicate with the player.
         * @param opponent       the object storing the quantities needed for the handshake.
         * @param playerMessage  the <code>PLAYER</code> message sent by the server,
         *                       containing the public key used to verify the digital signature
         *                       of the freshness proof.
         * @throws runtime_error    if an error occurred while receiving the message.
         * @throws SocketException  if an error occurred while sending an error message.
         */
        static void handleEndHandshakeP2P(const TcpSocket &socket,
                                          Player &opponent,
                                          const PlayerMessage &playerMessage);

        /**
         * Ends the handshake with the other player by sending
         * an <code>END_HANDSHAKE</code> message and deriving the symmetric session key.
         * @param socket             the socket used to communicate with the player.
         * @param myselfForOpponent  the object storing the quantities needed for the handshake.
         * @param digitalSignature   the digital signature tool.
         */
        static void endHandshakeP2P(const TcpSocket &socket,
                                    Player &myselfForOpponent,
                                    const DigitalSignature &digitalSignature);
    public:
        HandshakeHandler() = delete;
        ~HandshakeHandler() = delete;
        HandshakeHandler(const HandshakeHandler&) = delete;
        HandshakeHandler(HandshakeHandler&&) = delete;
        HandshakeHandler& operator=(const HandshakeHandler&) = delete;
        HandshakeHandler& operator=(HandshakeHandler&&) = delete;

        /**
         * Performs the handshake with the server.
         * @param socket            the socket used to communicate with the server.
         * @param myselfForServer   the object storing the quantities needed for the handshake.
         * @param username          the username of the client.
         * @param certificateStore  the certificate store used to verify untrusted certificates.
         * @param digitalSignature  the digital signature tool.
         * @return                  the first player list.
         * @throws runtime_error  if the handshake has failed.
         */
        static std::string doHandshakeWithServer(const TcpSocket &socket,
                                                 Player &myselfForServer,
                                                 const std::string &username,
                                                 const CertificateStore &certificateStore,
                                                 const DigitalSignature &digitalSignature);

        /**
         * Performs the handshake with another player.
         * @param myAddress         the IPv4 address of this client.
         * @param playerMessage     the <code>PLAYER</code> message sent by the server,
         *                          containing the IPv4 address of the opponent and the public key used
         *                          to verify the digital signature of the freshness proof.
         * @param digitalSignature  the digital signature tool.
         * @return                  a tuple containing a pointer to the socket used to communicate with
         *                          the other player, a pointer to the <code>Player</code> object
         *                          containing the handshake information and a boolean flag signaling
         *                          the success or failure of the handshake.
         *                          If the flag signals a failure, the two pointers are set to <code>nullptr</code>.
         */
        static P2PHandshakeResult doHandshakeWithPlayer(const std::string &myAddress,
                                                        const PlayerMessage &playerMessage,
                                                        const DigitalSignature &digitalSignature);
};

}

#endif //INC_4INAROW_HANDSHAKEHANDLER_H
