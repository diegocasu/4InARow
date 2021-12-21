#ifndef INC_4INAROW_MATCHMAKINGCLIENTHANDLER_H
#define INC_4INAROW_MATCHMAKINGCLIENTHANDLER_H

#include "Handler.h"
#include <PlayerMessage.h>

namespace fourinarow {

/**
 * Class representing a handler for messages sent by a player in the <code>MATCHMAKING</code> status.
 */
class MatchmakingClientHandler : public Handler {
    private:
        /**
         * Checks if the received message is a valid challenge response,
         * i.e. if the message is a <code>CHALLENGE_ACCEPTED/CHALLENGE_REFUSED</code> one and
         * the sender is the challenged player.
         * @param player  the player who sent the message.
         * @param type    the message type.
         * @return        true if the message is a valid challenge response, false otherwise.
         */
        static bool isValidChallengeResponse(const Player &player, uint8_t type);

        /**
         * Cancels a matchmaking putting the challenger and the challenged
         * in the <code>MATCHMAKING_INTERRUPTED</code> status. This method should be used
         * when the opponent must be found in the player list, because it
         * uses <code>findPlayerByUsername()</code>. If the two player objects
         * are directly available, use <code>cancelMatchmakingStatus()</code> on both.
         * If a failure occurs when finding the opponent in the list, the method
         * changes only the status of the given player.
         * @param player      the challenger or the challenged player.
         * @param playerList  the player list.
         * @param statusList  the status list.
         */
        static void cancelMatchmaking(Player &player, PlayerList &playerList, PlayerStatusList &statusList);

        /**
         * Sets the player status to <code>PLAYING</code>
         * @param player      the player.
         * @param statusList  the player status list.
         */
        static void setPlayingStatus(Player &player, PlayerStatusList &statusList);

        /**
         * Handles the reception of a <code>GOODBYE</code> message.
         * @param player      the player.
         * @param playerList  the player list.
         * @param statusList  the player status list.
         * @param removalList the player removal list.
         */
        static void handleGoodbye(Player &player,
                                  PlayerList &playerList,
                                  PlayerStatusList &statusList,
                                  PlayerRemovalList &removalList);

        /**
         * Forwards the challenge response to the challenger player.
         * If an error occurs while sending the message, the matchmaking
         * is automatically cancelled and no exceptions are thrown.
         * @param challengerSocket       the socket used to communicate with the challenger.
         * @param challengeResponseType  the challenge response type, either <code>CHALLENGE_ACCEPTED</code>
         *                               or <code>CHALLENGE_REFUSED</code>.
         * @param challengerPlayer       the challenger player.
         * @param challengedPlayer       the challenged player. Used only to cancel the matchmaking status
         *                               if an error occurs.
         * @param statusList             the player status list.
         * @param removalList            the player removal list.
         * @return                       true if the message is forwarded correctly, false otherwise.
         */
        static bool forwardChallengeResponse(const TcpSocket &challengerSocket,
                                             const uint8_t challengeResponseType,
                                             Player &challengerPlayer,
                                             Player &challengedPlayer,
                                             PlayerStatusList &statusList,
                                             PlayerRemovalList &removalList);

        /**
         * Sends a <code>PLAYER</code> message to the challenger player.
         * If an error occurs while sending the message, the matchmaking
         * is automatically cancelled and no exceptions are thrown.
         * @param challengerSocket       the socket used to communicate with the challenger.
         * @param message                the <code>PLAYER</code> message.
         * @param challengerPlayer       the challenger player.
         * @param challengedPlayer       the challenged player. Used only to cancel the matchmaking status
         *                               if an error occurs.
         * @param statusList             the player status list.
         * @param removalList            the player removal list.
         * @return                       true if the message is sent correctly, false otherwise.
         */
        static bool sendPlayerMessageToChallenger(const TcpSocket &challengerSocket,
                                                  const PlayerMessage &message,
                                                  Player &challengerPlayer,
                                                  Player &challengedPlayer,
                                                  PlayerStatusList &statusList,
                                                  PlayerRemovalList &removalList);

        /**
         * Handles the reception of either a <code>CHALLENGE_ACCEPTED</code>
         * or a <code>CHALLENGE_REFUSED</code> message.
         * @param challengedSocket       the socket used to communicate with the challenger.
         * @param challengeResponseType  the challenge response type.
         * @param challengedPlayer       the challenged player.
         * @param playerList             the player list.
         * @param statusList             the player status list.
         * @param removalList            the player removal list.
         */
        static void handleChallengeResponse(const TcpSocket &challengedSocket,
                                            const uint8_t challengeResponseType,
                                            Player &challengedPlayer,
                                            PlayerList &playerList,
                                            PlayerStatusList &statusList,
                                            PlayerRemovalList &removalList);

    public:
        MatchmakingClientHandler() = delete;
        ~MatchmakingClientHandler() = delete;
        MatchmakingClientHandler(const MatchmakingClientHandler&) = delete;
        MatchmakingClientHandler(MatchmakingClientHandler&&) = delete;
        MatchmakingClientHandler &operator=(const MatchmakingClientHandler&) = delete;
        MatchmakingClientHandler &operator=(MatchmakingClientHandler&&) = delete;

        /**
         * Handles a message sent by a player in the <code>MATCHMAKING</code> status.
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

#endif //INC_4INAROW_MATCHMAKINGCLIENTHANDLER_H
