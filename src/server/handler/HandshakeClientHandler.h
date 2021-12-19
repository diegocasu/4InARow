#ifndef INC_4INAROW_HANDSHAKECLIENTHANDLER_H
#define INC_4INAROW_HANDSHAKECLIENTHANDLER_H

#include "Handler.h"
#include <DigitalSignature.h>

namespace fourinarow {

/**
 * Class representing a handler for messages sent by a player
 * in the <code>HANDSHAKE</code> status.
 */
class HandshakeClientHandler : public Handler {
    private:
        /**
         * Implements the first part of the handler, in which an <code>END_HANDSHAKE</code>
         * message must be received. In this part, messages are exchanged in cleartext.
         * If the handshake succeeds, the player is set as <code>AVAILABLE</code> and
         * the symmetric session key is derived.
         * If a failure occurs, the player is put into the removal list.
         * @param socket            the socket used to communicate.
         * @param player            the player.
         * @param statusList        the player status list.
         * @param removalList       the player removal list.
         * @return                  true if the connection with the client can continue,
         *                          false if it must be closed.
         */
        static bool handleEndHandshake(const TcpSocket &socket,
                                       Player &player,
                                       PlayerStatusList &statusList,
                                       PlayerRemovalList &removalList);

        /**
         * Implements the second part of the handler, in which a </code>PLAYER_LIST</code>
         * message is sent. In this part, messages are exchanged in ciphertext.
         * If a failure occurs, the player is put into the removal list.
         * @param socket       the socket used to communicate.
         * @param player       the player.
         * @param statusList   the player status list.
         * @param removalList  the player removal list.
         */
        static void handleSendPlayerList(const TcpSocket &socket,
                                         Player &player,
                                         PlayerStatusList &statusList,
                                         PlayerRemovalList &removalList);
    public:
        HandshakeClientHandler() = delete;
        ~HandshakeClientHandler() = delete;
        HandshakeClientHandler(const HandshakeClientHandler&) = delete;
        HandshakeClientHandler(HandshakeClientHandler&&) = delete;
        HandshakeClientHandler& operator=(const HandshakeClientHandler&) = delete;
        HandshakeClientHandler& operator=(HandshakeClientHandler&&) = delete;

        /**
         * Handles a message sent by a player in the <code>HANDSHAKE</code> status.
         * If an unrecoverable error is detected, the player is put into the removal list.
         * @param socket            the socket used to communicate.
         * @param player            the player.
         * @param statusList        the player status list.
         * @param removalList       the player removal list.
         */
        static void handle(const TcpSocket &socket,
                           Player &player,
                           PlayerStatusList &statusList,
                           PlayerRemovalList &removalList);
};

}

#endif //INC_4INAROW_HANDSHAKECLIENTHANDLER_H
