#include <iostream>
#include <Utils.h>
#include <SerializationException.h>
#include <SocketException.h>
#include <CryptoException.h>
#include "PlayingClientHandler.h"

namespace fourinarow {

void PlayingClientHandler::setAvailableStatus(Player &player, PlayerStatusList &statusList) {
    player.setStatus(Player::Status::AVAILABLE);
    statusList[player.getUsername()] = Player::Status::AVAILABLE;
}

void PlayingClientHandler::handle(const TcpSocket &socket,
                                  Player &player,
                                  PlayerStatusList &statusList,
                                  PlayerRemovalList &removalList) {
    try {
        auto encryptedMessage = socket.receive();
        auto message = authenticateAndDecrypt(encryptedMessage, player);
        auto type = getMessageType<SerializationException>(message);

        if (type == END_GAME) {
            std::cout << "Received an END_GAME message. Making the client available again for playing" << std::endl;
            setAvailableStatus(player, statusList);
            return;
        }

        std::cerr << "Protocol violation: received " << convertMessageType(type) << std::endl;
        InfoMessage protocolViolation(PROTOCOL_VIOLATION);
        socket.send(encryptAndAuthenticate(&protocolViolation, player));
    } catch (const SocketException &exception) {
        std::cerr << "Error while handling the message: " << exception.what() << std::endl;
        removalList.insert(player.getUsername());

    } catch (const SerializationException &exception) {
        std::cerr << "Error while handling the message: " << exception.what() << std::endl;
        failSafeSendErrorInCiphertext(socket, player, InfoMessage(MALFORMED_MESSAGE), removalList);

    } catch (const CryptoException &exception) {
        std::cerr << "Error while handling the message: " << exception.what() << std::endl;
        failSafeSendErrorInCiphertext(socket, player, InfoMessage(MALFORMED_MESSAGE), removalList);

    } catch (const std::exception &exception) {
        std::cerr << "Error while handling the message: " << exception.what() << std::endl;
        failSafeSendErrorInCiphertext(socket, player, InfoMessage(INTERNAL_ERROR), removalList);
        removalList.insert(player.getUsername());
    }
}

}
