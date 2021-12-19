#ifndef INC_4INAROW_AVAILABLECLIENTHANDLER_H
#define INC_4INAROW_AVAILABLECLIENTHANDLER_H

#include "Handler.h"

namespace fourinarow {

/**
 * Class representing a handler for messages sent by a player
 * in the <code>AVAILABLE</code> status.
 */
class AvailableClientHandler : public Handler {
    private:
        /**
         * Checks if the challenge is valid, namely if:
         * 1) the challenged and the challenger are different players;
         * 2) the challenged is online and <code>AVAILABLE</code>.
         * @param challenger  the challenger player.
         * @param challenged  the challenged player.
         * @param statusList  the player status list.
         * @return            true if the challenge is valid, false otherwise.
         */
        static bool isValidChallenge(const std::string &challenger,
                                     const std::string &challenged,
                                     PlayerStatusList &statusList);

        /**
         * Handles the reception of a <code>CHALLENGE</code> message.
         * @param challengerSocket  the socket of the challenger.
         * @param message           the <code>CHALLENGE</code> message in binary format.
         * @param challenger        the challenger player.
         * @param playerList        the player list.
         * @param statusList        the player status list.
         * @param removalList       the player removal list.
         */
        static void handleChallengeMessage(const TcpSocket &challengerSocket,
                                           std::vector<unsigned char> &message,
                                           Player &challenger,
                                           PlayerList &playerList,
                                           PlayerStatusList &statusList,
                                           PlayerRemovalList &removalList);

        /**
         * Handles the reception of a <code>REQ_PLAYER_LIST</code> message.
         * @param socket      the socket used to communicate.
         * @param player      the player.
         * @param statusList  the player status list.
         * @throws  SocketException  if an error occurred while sending the response.
         * @throws  CryptoException  if an error occurred while encrypting the response,
         *                           or the maximum sequence number has been reached.
         */
        static void handleSendPlayerList(const TcpSocket &socket,
                                         Player &player,
                                         PlayerStatusList &statusList);
        /**
         * Handles the reception of a <code>GOODBYE</code> message.
         * @param player       the player.
         * @param removalList  the player removal list.
         */
        static void handleGoodbye(Player &player, PlayerRemovalList &removalList);

    public:
        AvailableClientHandler() = delete;
        ~AvailableClientHandler() = delete;
        AvailableClientHandler(const AvailableClientHandler&) = delete;
        AvailableClientHandler(AvailableClientHandler&&) = delete;
        AvailableClientHandler& operator=(const AvailableClientHandler&) = delete;
        AvailableClientHandler& operator=(AvailableClientHandler&&) = delete;

        /**
         * Handles a message sent by a player in the <code>AVAILABLE</code> status.
         * @param socket       the socket used to communicate.
         * @param player       the player.
         * @param playerList   the player list.
         * @param statusList   the player status list.
         * @param removalList  the player removal list.
         */
        static void handle(const TcpSocket &socket,
                           Player &player,
                           PlayerList &playerList,
                           PlayerStatusList &statusList,
                           PlayerRemovalList &removalList);
};

}

#endif //INC_4INAROW_AVAILABLECLIENTHANDLER_H
