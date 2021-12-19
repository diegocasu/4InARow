#include <iostream>
#include <Utils.h>
#include <SocketException.h>
#include <CryptoException.h>
#include <SerializationException.h>
#include <CSPRNG.h>
#include <DigitalSignature.h>
#include "MatchmakingClientHandler.h"

namespace fourinarow {

bool MatchmakingClientHandler::isValidChallengeResponse(const Player &player, uint8_t type) {
    return !player.isMatchmakingInitiator() && (type == CHALLENGE_ACCEPTED || type == CHALLENGE_REFUSED);
}

void MatchmakingClientHandler::cancelMatchmaking(Player &player, PlayerList &playerList, PlayerStatusList &statusList) {
    /*
     * The reset for the player whose object is passed directly must be done as the last step.
     * Indeed, cancelMatchmakingStatus() clears the field matchmakingPlayer,
     * and would cause a failure of findPlayerByUsername().
     * The try-catch avoids crashing the server in case the matchmaking player is not found in the player
     * list, but this should never happen in a correct implementation of the server (either both are in
     * MATCHMAKING state, or the matchmaking has already been cancelled).
     */
    try {
        cancelMatchmakingStatus(findPlayerByUsername(playerList, player.getMatchmakingPlayer()).second, statusList);
    } catch (const std::exception &exception) {
        std::cout << "Critical error: cannot cancel the matchmaking status of " << player.getMatchmakingPlayer() << ". ";
        std::cout << "Reason: " << exception.what() << std::endl;
    }

    cancelMatchmakingStatus(player, statusList);
}

void MatchmakingClientHandler::setPlayingStatus(Player &player, PlayerStatusList &statusList) {
    player.setStatus(Player::Status::PLAYING);
    statusList[player.getUsername()] = Player::Status::PLAYING;
}

void MatchmakingClientHandler::handleGoodbye(Player &player,
                                             PlayerList &playerList,
                                             PlayerStatusList &statusList,
                                             PlayerRemovalList &removalList) {
    std::cout << "Received a GOODBYE message. Disconnecting the client" << std::endl;
    cancelMatchmaking(player, playerList, statusList);
    removalList.insert(player.getUsername());
    return;
}

bool MatchmakingClientHandler::forwardChallengeResponse(const TcpSocket &challengerSocket,
                                                        const uint8_t challengeResponseType,
                                                        Player &challengerPlayer,
                                                        Player &challengedPlayer,
                                                        PlayerStatusList &statusList,
                                                        PlayerRemovalList &removalList) {
    std::cout << "Forwarding the message to the challenger '" << challengerPlayer.getUsername() << "'" << std::endl;

    try {
        InfoMessage challengeResponse(challengeResponseType);
        challengerSocket.send(encryptAndAuthenticate(&challengeResponse, challengerPlayer));
        return true;
    } catch (const std::exception &exception) {
        std::cerr << "Error while forwarding the message: " << exception.what() << std::endl;

        /*
         * Rollback and removal of the challenger player.
         * (either a socket error occurred or the max sequence number has been reached).
         */
        cancelMatchmakingStatus(challengedPlayer, statusList);
        cancelMatchmakingStatus(challengerPlayer, statusList);
        removalList.insert(challengerPlayer.getUsername());
        return false;
    }
}

bool MatchmakingClientHandler::sendPlayerMessageToChallenger(const TcpSocket &challengerSocket,
                                                             const PlayerMessage &message,
                                                             Player &challengerPlayer,
                                                             Player &challengedPlayer,
                                                             PlayerStatusList &statusList,
                                                             PlayerRemovalList &removalList) {
    std::cout << "Sending a PLAYER message to the challenger '" << challengerPlayer.getUsername() << "'" << std::endl;

    try {
        challengerSocket.send(encryptAndAuthenticate(&message, challengerPlayer));
        return true;
    } catch (const std::exception &exception) {
        std::cerr << "Error while sending the message: " << exception.what() << std::endl;

        /*
         * Rollback and removal of the challenger player
         * (either a socket error occurred or the max sequence number has been reached).
         */
        cancelMatchmakingStatus(challengedPlayer, statusList);
        cancelMatchmakingStatus(challengerPlayer, statusList);
        removalList.insert(challengerPlayer.getUsername());
        return false;
    }
}

void MatchmakingClientHandler::handleChallengeResponse(const TcpSocket &challengedSocket,
                                                       const uint8_t challengeResponseType,
                                                       Player &challengedPlayer,
                                                       PlayerList &playerList,
                                                       PlayerStatusList &statusList,
                                                       PlayerRemovalList &removalList) {
    /*
     * The exceptions caused by the challenged player are not caught in this method,
     * but handled in the caller method handle(), together with the others.
     * The exceptions caused by the challenger player are caught and handled locally
     * in the methods forwardChallengeResponse() and sendPlayerMessageToChallenger(),
     * to avoid disconnecting the challenged for errors not linked to her connection.
     */
    std::cout << "Received a " << convertMessageType(challengeResponseType) << " message\n";
    auto &iterator = findPlayerByUsername(playerList, challengedPlayer.getMatchmakingPlayer());

    if (!forwardChallengeResponse(iterator.first, challengeResponseType, iterator.second, challengedPlayer, statusList, removalList)) {
        return;
    }

    if (challengeResponseType == CHALLENGE_REFUSED) {
        cancelMatchmakingStatus(challengedPlayer, statusList);
        cancelMatchmakingStatus(iterator.second, statusList);
        return;
    }

    std::string challengerPublicKeyPath = SERVER_PLAYERS_FOLDER + iterator.second.getUsername() + SERVER_PLAYER_KEY_SUFFIX;
    std::string challengedPublicKeyPath = SERVER_PLAYERS_FOLDER + challengedPlayer.getUsername() + SERVER_PLAYER_KEY_SUFFIX;
    auto challengerFirstToPlay = CSPRNG::nextBool();

    PlayerMessage toChallenger(challengedSocket.getDestinationAddress(),
                               DigitalSignature::serializePublicKey(challengedPublicKeyPath),
                               challengerFirstToPlay);
    PlayerMessage toChallenged(iterator.first.getDestinationAddress(),
                               DigitalSignature::serializePublicKey(challengerPublicKeyPath),
                               !challengerFirstToPlay);

    std::cout << "Sending a PLAYER message to the challenged '" << challengedPlayer.getUsername() << "'" << std::endl;
    challengedSocket.send(encryptAndAuthenticate(&toChallenged, challengedPlayer));

    if (!sendPlayerMessageToChallenger(iterator.first, toChallenger, iterator.second, challengedPlayer, statusList, removalList)) {
        return;
    }

    cancelMatchmakingStatus(challengedPlayer, statusList);
    cancelMatchmakingStatus(iterator.second, statusList);
    setPlayingStatus(challengedPlayer, statusList);
    setPlayingStatus(iterator.second, statusList);
}

void MatchmakingClientHandler::handle(const TcpSocket &socket,
                                      Player &player,
                                      PlayerList &playerList,
                                      PlayerStatusList &statusList,
                                      PlayerRemovalList &removalList) {
    try {
        auto encryptedMessage = socket.receive();
        auto message = authenticateAndDecrypt(encryptedMessage, player);
        auto type = getMessageType<SerializationException>(message);

        if (type == GOODBYE) {
            handleGoodbye(player, playerList, statusList, removalList);
            return;
        }

        if (isValidChallengeResponse(player, type)) {
            handleChallengeResponse(socket, type, player, playerList, statusList, removalList);
            return;
        }

        std::cerr << "Protocol violation: received " << convertMessageType(type) << std::endl;
        cancelMatchmaking(player, playerList, statusList);
        InfoMessage protocolViolation(PROTOCOL_VIOLATION);
        socket.send(encryptAndAuthenticate(&protocolViolation, player));
    } catch (const SocketException &exception) {
        std::cerr << "Error while handling the message: " << exception.what() << std::endl;
        cancelMatchmaking(player, playerList, statusList);
        removalList.insert(player.getUsername());

    } catch (const SerializationException &exception) {
        std::cerr << "Error while handling the message: " << exception.what() << std::endl;
        cancelMatchmaking(player, playerList, statusList);
        failSafeSendErrorInCiphertext(socket, player, InfoMessage(MALFORMED_MESSAGE), removalList);

    } catch (const CryptoException &exception) {
        std::cerr << "Error while handling the message: " << exception.what() << std::endl;
        cancelMatchmaking(player, playerList, statusList);
        failSafeSendErrorInCiphertext(socket, player, InfoMessage(MALFORMED_MESSAGE), removalList);

    } catch (const std::exception &exception) {
        std::cerr << "Error while handling the message: " << exception.what() << std::endl;
        cancelMatchmaking(player, playerList, statusList);
        failSafeSendErrorInCiphertext(socket, player, InfoMessage(INTERNAL_ERROR), removalList);
        removalList.insert(player.getUsername());
    }
}

}
