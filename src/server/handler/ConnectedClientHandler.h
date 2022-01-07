#ifndef INC_4INAROW_CONNECTEDCLIENTHANDLER_H
#define INC_4INAROW_CONNECTEDCLIENTHANDLER_H

#include "Handler.h"
#include <ClientHello.h>
#include <DigitalSignature.h>

namespace fourinarow {

/**
 * Class representing a handler for messages sent by a player in the <code>CONNECTED</code> status.
 */
class ConnectedClientHandler : public Handler {
    private:
        /**
         * Checks if the given username belongs to a player already connected to the server,
         * namely to a player at least in the <code>CONNECTED</code> status.
         * @param statusList  the player status list.
         * @param username    the username.
         * @return            true if the player is already connected, false otherwise.
         */
        static bool isPlayerAlreadyConnected(const PlayerStatusList &statusList, const std::string &username);

        /**
         * Checks if the given username is registered, namely if
         * the server has a corresponding public key file in its dedicated folder.
         * @param username  the username of the client.
         * @return          true if the username is registered, false otherwise.
         */
        static bool isUsernameRegistered(const std::string &username);

        /**
         * Updates the given <code>Player</code> object by:
         * 1) setting the username;
         * 2) setting the status to <code>HANDSHAKE</code>;
         * 3) generating the server nonce and the server keys;
         * 4) setting the client nonce;
         * 5) generating the proof of freshness of the server.
         * @param player       the player.
         * @param statusList   the player status list.
         * @param clientHello  the <code>CLIENT_HELLO</code> message.
         * @param certificate  the certificate of the server.
         * @throws CryptoException         if an error occurs while generating the nonce and the keys,
         *                                 or while generating the proof of freshness.
         * @throws SerializationException  if the message contains an invalid username, nonce or key.
         */
        static void updatePlayerQuantities(Player &player,
                                           PlayerStatusList &statusList,
                                           const ClientHello &clientHello);
    public:
        ConnectedClientHandler() = delete;
        ~ConnectedClientHandler() = delete;
        ConnectedClientHandler(const ConnectedClientHandler&) = delete;
        ConnectedClientHandler(ConnectedClientHandler&&) = delete;
        ConnectedClientHandler& operator=(const ConnectedClientHandler&) = delete;
        ConnectedClientHandler& operator=(ConnectedClientHandler&&) = delete;

        /**
         * Handles a message sent by a player in the <code>CONNECTED</code> status.
         * If an unrecoverable error is detected, the player is put into the removal list.
         * @param socket            the socket used to communicate.
         * @param player            the player.
         * @param statusList        the player status list.
         * @param removalList       the player removal list.
         * @param certificate       the certificate of the server.
         * @param digitalSignature  the digital signature tool of the server.
         */
        static void handle(const TcpSocket &socket,
                           Player &player,
                           PlayerStatusList &statusList,
                           PlayerRemovalList &removalList,
                           const std::vector<unsigned char> &certificate,
                           const DigitalSignature &digitalSignature);
};

}

#endif //INC_4INAROW_CONNECTEDCLIENTHANDLER_H
