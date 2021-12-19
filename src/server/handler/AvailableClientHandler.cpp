#include <iostream>
#include <Utils.h>
#include <SerializationException.h>
#include <SocketException.h>
#include <CryptoException.h>
#include <PlayerListMessage.h>
#include <Challenge.h>
#include "AvailableClientHandler.h"

namespace fourinarow {

bool AvailableClientHandler::isValidChallenge(const std::string &challenger,
                                              const std::string &challenged,
                                              PlayerStatusList &statusList) {
    return (challenger != challenged) &&
           (statusList.count(challenged) != 0) &&
           (statusList[challenged] == Player::Status::AVAILABLE);
}

void AvailableClientHandler::handleChallengeMessage(const TcpSocket &challengerSocket,
                                                    std::vector<unsigned char> &message,
                                                    Player &challenger,
                                                    PlayerList &playerList,
                                                    PlayerStatusList &statusList,
                                                    PlayerRemovalList &removalList) {
    /*
     * The exceptions caused by the challenger player are not caught in this method,
     * but handled in the caller method handle(), together with the others.
     * The exceptions caused by the challenged player are caught and handled in this method,
     * to avoid disconnecting the challenger for errors not linked to her connection.
     */
    std::cout << "Received a CHALLENGE message. Forwarding the message to the challenged player" << std::endl;
    Challenge challengeMessage;
    challengeMessage.deserialize(message);

    if (!isValidChallenge(challenger.getUsername(), challengeMessage.getUsername(), statusList)) {
        std::cout << "The player '" << challengeMessage.getUsername() << "' is not available" << std::endl;
        InfoMessage notAvailable(PLAYER_NOT_AVAILABLE);
        challengerSocket.send(encryptAndAuthenticate(&notAvailable, challenger));
        return;
    }

    setMatchmakingStatus(challenger, statusList, challengeMessage.getUsername(), true);
    auto &iterator = findPlayerByUsername(playerList, challengeMessage.getUsername());

    try {
        Challenge challengePropagationMessage(challenger.getUsername());
        iterator.first.send(encryptAndAuthenticate(&challengePropagationMessage, iterator.second));
        setMatchmakingStatus(iterator.second, statusList, challenger.getUsername(), false);
        std::cout << "CHALLENGE message forwarded to '" << challengeMessage.getUsername() << '\'' << std::endl;
    } catch (const std::exception &exception) {
        std::cerr << "Error while forwarding the message: " << exception.what() << std::endl;

        /*
         * Rollback and removal of the challenged player
         * (either a socket error occurred or the max sequence number has been reached).
         */
        cancelMatchmakingStatus(challenger,statusList);
        cancelMatchmakingStatus(iterator.second, statusList);
        removalList.insert(iterator.second.getUsername());

        // Notify the challenger about the failure. If these statements throw, the exceptions are caught in handle().
        InfoMessage notAvailable(PLAYER_NOT_AVAILABLE);
        challengerSocket.send(encryptAndAuthenticate(&notAvailable, challenger));
    }
}

void AvailableClientHandler::handleSendPlayerList(const TcpSocket &socket,
                                                  Player &player,
                                                  PlayerStatusList &statusList) {
    std::cout << "Received a REQ_PLAYER_LIST message. Sending back a PLAYER_LIST message" << std::endl;
    PlayerListMessage playerListMessage(generatePlayerList(statusList, player.getUsername()));
    socket.send(encryptAndAuthenticate(&playerListMessage, player));
}

void AvailableClientHandler::handleGoodbye(Player &player, PlayerRemovalList &removalList) {
    std::cout << "Received a GOODBYE message. Disconnecting the client" << std::endl;
    removalList.insert(player.getUsername());
}

void AvailableClientHandler::handle(const TcpSocket &socket,
                                    Player &player,
                                    PlayerList &playerList,
                                    PlayerStatusList &statusList,
                                    PlayerRemovalList &removalList) {
    try {
        auto encryptedMessage = socket.receive();
        auto message = authenticateAndDecrypt(encryptedMessage, player);
        auto type = getMessageType<SerializationException>(message);

        if (type == GOODBYE) {
            handleGoodbye(player, removalList);
            return;
        }

        if (type == REQ_PLAYER_LIST) {
            handleSendPlayerList(socket, player, statusList);
            return;
        }

        if (type == CHALLENGE) {
            handleChallengeMessage(socket, message, player, playerList, statusList, removalList);
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
