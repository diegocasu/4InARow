#include <iostream>
#include <Utils.h>
#include <SocketException.h>
#include <SerializationException.h>
#include <InfoMessage.h>
#include <EndHandshake.h>
#include <PlayerListMessage.h>
#include "HandshakeClientHandler.h"

namespace fourinarow {

bool HandshakeClientHandler::handleEndHandshake(const TcpSocket &socket,
                                                Player &player,
                                                PlayerStatusList &statusList,
                                                PlayerRemovalList &removalList,
                                                const DigitalSignature &digitalSignature) {
    std::cout << "Handshake: handling an END_HANDSHAKE message" << std::endl;

    try {
        auto message = socket.receive();
        auto type = getMessageType<SerializationException>(message);

        if (type != END_HANDSHAKE) {
            std::cerr << "Protocol violation: received " << convertMessageType(type) << std::endl;
            socket.send(InfoMessage(PROTOCOL_VIOLATION).serialize());
            removalList.insert(player.getUsername());
            return false;
        }

        EndHandshake endHandshake;
        endHandshake.deserialize(message);

        std::string userPublicKeyPath = SERVER_PLAYERS_FOLDER + player.getUsername() + SERVER_PLAYER_KEY_SUFFIX;
        if (!digitalSignature.verify(player.getFreshnessProof(), endHandshake.getDigitalSignature(), userPublicKeyPath)) {
            std::cerr << "Aborting the handshake: received an invalid proof of freshness" << std::endl;
            socket.send(InfoMessage(MALFORMED_MESSAGE).serialize());
            removalList.insert(player.getUsername());
            return false;
        }

        player.setStatus(Player::Status::AVAILABLE);
        statusList[player.getUsername()] = Player::Status::AVAILABLE;
        player.initCipher();
        return true;
    } catch (const SocketException &exception) {
        std::cerr << "Error while finalizing the handshake: " << exception.what() << std::endl;

    } catch (const SerializationException &exception) {
        std::cerr << "Error while finalizing the handshake: " << exception.what() << std::endl;
        failSafeSendErrorInCleartext(socket, InfoMessage(MALFORMED_MESSAGE));

    } catch (const std::exception &exception) {
        std::cerr << "Error while finalizing the handshake: " << exception.what() << std::endl;
        failSafeSendErrorInCleartext(socket, InfoMessage(INTERNAL_ERROR));
    }
    removalList.insert(player.getUsername());
    return false;
}

void HandshakeClientHandler::handleSendPlayerList(const TcpSocket &socket,
                                                  Player &player,
                                                  PlayerStatusList &statusList,
                                                  PlayerRemovalList &removalList) {
    std::cout << "Handshake finished. Sending a PLAYER_LIST message" << std::endl;

    try {
        PlayerListMessage playerListMessage(generatePlayerList(statusList, player.getUsername()));
        socket.send(encryptAndAuthenticate(&playerListMessage, player));
        return;
    } catch (const SocketException &exception) {
        std::cerr << "Error while sending the player list: " << exception.what() << std::endl;

    } catch (const std::exception &exception) {
        std::cerr << "Error while sending the player list: " << exception.what() << std::endl;
        failSafeSendErrorInCiphertext(socket, player, InfoMessage(INTERNAL_ERROR), removalList);
    }
    removalList.insert(player.getUsername());
}

void HandshakeClientHandler::handle(const TcpSocket &socket,
                                    Player &player,
                                    PlayerStatusList &statusList,
                                    PlayerRemovalList &removalList,
                                    const DigitalSignature &digitalSignature) {
    if (!handleEndHandshake(socket, player, statusList, removalList, digitalSignature)) {
        return;
    }
    handleSendPlayerList(socket, player, statusList, removalList);
}

}
