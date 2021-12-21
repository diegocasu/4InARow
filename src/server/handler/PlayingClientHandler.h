#ifndef INC_4INAROW_PLAYINGCLIENTHANDLER_H
#define INC_4INAROW_PLAYINGCLIENTHANDLER_H

#include "Handler.h"

namespace fourinarow {

/**
 * Class representing a handler for messages sent by a player in the <code>PLAYING</code> status.
 */
class PlayingClientHandler : public Handler {
    private:
        /**
         * Sets a player as <code>AVAILABLE</code>.
         * @param player      the player.
         * @param statusList  the player status list.
         */
        static void setAvailableStatus(Player &player, PlayerStatusList &statusList);

    public:
        PlayingClientHandler() = delete;
        ~PlayingClientHandler() = delete;
        PlayingClientHandler(const PlayingClientHandler&) = delete;
        PlayingClientHandler(PlayingClientHandler&&) = delete;
        PlayingClientHandler &operator=(const PlayingClientHandler&) = delete;
        PlayingClientHandler &operator=(PlayingClientHandler&&) = delete;

        /**
         * Handles a message sent by a player in the <code>PLAYING</code> status.
         * @param socket       the socket used to communicate.
         * @param player       the player.
         * @param statusList   the player status list.
         * @param removalList  the player removal list.
         */
        static void handle(const TcpSocket &socket,
                           Player &player,
                           PlayerStatusList &statusList,
                           PlayerRemovalList &removalList);

};

}

#endif //INC_4INAROW_PLAYINGCLIENTHANDLER_H
