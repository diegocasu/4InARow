#ifndef INC_4INAROW_NEWCLIENTHANDLER_H
#define INC_4INAROW_NEWCLIENTHANDLER_H

#include "Handler.h"
#include <InputMultiplexer.h>

namespace fourinarow {

/**
 * Class representing a handler for new connections on the hello socket.
 */
class NewClientHandler : public Handler {
    public:
        NewClientHandler() = delete;
        ~NewClientHandler() = delete;
        NewClientHandler(const NewClientHandler&) = delete;
        NewClientHandler(NewClientHandler&&) = delete;
        NewClientHandler& operator=(const NewClientHandler&) = delete;
        NewClientHandler& operator=(NewClientHandler&&) = delete;

        /**
         * Handles a new connection on the hello socket.
         * @param helloSocket  the hello socket.
         * @param multiplexer  the input multiplexer managing the server sockets.
         * @param playerList   the player list.
         */
        static void handle(TcpSocket &helloSocket, InputMultiplexer &multiplexer, PlayerList &playerList);
};

}

#endif //INC_4INAROW_NEWCLIENTHANDLER_H
