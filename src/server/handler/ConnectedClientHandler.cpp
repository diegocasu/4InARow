#include <iostream>
#include <Utils.h>
#include <SerializationException.h>
#include <SocketException.h>
#include <ServerHello.h>
#include "ConnectedClientHandler.h"

namespace fourinarow {

bool ConnectedClientHandler::isPlayerAlreadyConnected(const PlayerStatusList &statusList, const std::string &username) {
    return statusList.count(username) != 0;
}

bool ConnectedClientHandler::isUsernameRegistered(const std::string &username) {
    std::string path(SERVER_PLAYERS_FOLDER + username + SERVER_PLAYER_KEY_SUFFIX);
    FILE *file = fopen(path.data(), "r");

    if (!file) {
        return false;
    }

    fclose(file);
    return true;
}

void ConnectedClientHandler::updatePlayerQuantities(Player &player,
                                                    PlayerStatusList &statusList,
                                                    const ClientHello &clientHello,
                                                    const std::vector<unsigned char> &certificate) {
    player.setUsername(clientHello.getUsername());
    player.setStatus(Player::Status::HANDSHAKE);
    statusList[player.getUsername()] = Player::Status::HANDSHAKE;

    player.generateServerNonce();
    player.generateServerKeys();
    player.setClientNonce(clientHello.getNonce());
    player.setClientPublicKey(clientHello.getPublicKey());

    player.generateFreshnessProof(certificate);
}

void ConnectedClientHandler::handle(const TcpSocket &socket,
                                    Player &player,
                                    PlayerStatusList &statusList,
                                    PlayerRemovalList &removalList,
                                    const std::vector<unsigned char> &certificate,
                                    const DigitalSignature &digitalSignature) {
    std::cout << "Handshake: handling a CLIENT_HELLO message" << std::endl;

    try {
        auto message = socket.receive();
        auto type = getMessageType<SerializationException>(message);

        if (type != CLIENT_HELLO) {
            std::cerr << "Protocol violation: received " << convertMessageType(type) << std::endl;
            socket.send(InfoMessage(PROTOCOL_VIOLATION).serialize());
            removalList.insert(player.getUsername()); // The client is anonymous: an empty string ("") is inserted.
            return;
        }

        ClientHello clientHello;
        clientHello.deserialize(message);

        if (isPlayerAlreadyConnected(statusList, clientHello.getUsername())) {
            std::cerr << "A player with username '" << clientHello.getUsername() << "' is already connected. ";
            std::cerr << "Disconnecting the client." << std::endl;
            socket.send(InfoMessage(PLAYER_ALREADY_CONNECTED).serialize());
            removalList.insert(player.getUsername()); // The client is anonymous: an empty string ("") is inserted.
            return;
        }

        if (!isUsernameRegistered(clientHello.getUsername())) {
            std::cerr << "The player '" << clientHello.getUsername() << "' is not registered. ";
            std::cerr << "Disconnecting the client." << std::endl;
            socket.send(InfoMessage(PLAYER_NOT_REGISTERED).serialize());
            removalList.insert(player.getUsername()); // The client is anonymous: an empty string ("") is inserted.
            return;
        }

        updatePlayerQuantities(player, statusList, clientHello, certificate);
        std::cout << "Handshake: responding with a SERVER_HELLO message" << std::endl;
        socket.send(ServerHello(certificate,
                                player.getServerNonce(),
                                player.getServerPublicKey(),
                                digitalSignature.sign(player.getFreshnessProof())
                                ).serialize());
        return;
    } catch (const SocketException &exception) {
        std::cerr << "Error while performing the handshake: " << exception.what() << std::endl;

    } catch (const SerializationException &exception) {
        std::cerr << "Error while performing the handshake: " << exception.what() << std::endl;
        failSafeSendErrorInCleartext(socket, InfoMessage(MALFORMED_MESSAGE));

    } catch (const std::exception &exception) {
        std::cerr << "Error while performing the handshake: " << exception.what() << std::endl;
        failSafeSendErrorInCleartext(socket, InfoMessage(INTERNAL_ERROR));
    }
    removalList.insert(player.getUsername());
}

}
