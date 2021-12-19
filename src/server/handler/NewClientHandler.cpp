#include <iostream>
#include "NewClientHandler.h"

namespace fourinarow {

void NewClientHandler::handle(TcpSocket &helloSocket, InputMultiplexer &multiplexer, PlayerList &playerList) {
    std::cout << "Hello socket: new connection request" << std::endl;
    auto newDescriptor = -1; // Used for rollback.

    try {
        auto newClientSocket = helloSocket.accept();
        newDescriptor = newClientSocket.getDescriptor();

        std::cout << "Accepting a new connection from " << newClientSocket.getFullDestinationAddress() << std::endl;
        Player newPlayer;
        newPlayer.setStatus(Player::Status::CONNECTED);

        multiplexer.addDescriptor(newClientSocket.getDescriptor());
        playerList[std::move(newClientSocket)] = std::move(newPlayer);
    } catch (const std::exception &exception) {
        std::cerr << "Impossible to accept the connection: " << exception.what() << std::endl;
        if (newDescriptor >= 0) {
            multiplexer.removeDescriptor(newDescriptor); // Rollback in case the insertion in playerList fails.
        }
    }
}

}
