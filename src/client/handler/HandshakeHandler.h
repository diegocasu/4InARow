#ifndef INC_4INAROW_HANDSHAKEHANDLER_H
#define INC_4INAROW_HANDSHAKEHANDLER_H

#include "Handler.h"
#include <CertificateStore.h>

namespace fourinarow {

/**
 * Class representing a handler for the handshake with the server
 * or with another client.
 */
class HandshakeHandler : public Handler {
    private:
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
};

}

#endif //INC_4INAROW_HANDSHAKEHANDLER_H
