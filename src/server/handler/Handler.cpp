#include <iostream>
#include <string.h>
#include <arpa/inet.h>
#include <Utils.h>
#include "Handler.h"

namespace fourinarow {

std::string Handler::generatePlayerList(const PlayerStatusList &statusList, const std::string &excludedUsername) {
    std::string players;

    for (auto &iterator : statusList) {
        if (iterator.first == excludedUsername) {
            continue;
        }

        if (iterator.second == Player::Status::AVAILABLE) {
            players += (iterator.first + ';');
        }
    }

    return players;
}

std::vector<unsigned char> Handler::encryptAndAuthenticate(const Message *message, Player &player) {
    // Generate the additional authenticated data using the sequence number.
    uint32_t sequenceNumber = htonl(player.getSequenceNumber());
    std::vector<unsigned char> aad(sizeof(sequenceNumber));
    memcpy(aad.data(), &sequenceNumber, sizeof(sequenceNumber));

    auto plaintext = message->serialize();
    auto authenticatedCiphertext = player.getCipher().encrypt(plaintext, aad);
    cleanse(plaintext);

    player.incrementSequenceNumber();
    return authenticatedCiphertext;
}

std::vector<unsigned char> Handler::authenticateAndDecrypt(std::vector<unsigned char> &message, Player &player) {
    // Generate the additional authenticated data using the sequence number.
    uint32_t sequenceNumber = htonl(player.getSequenceNumber());
    std::vector<unsigned char> aad(sizeof(sequenceNumber));
    memcpy(aad.data(), &sequenceNumber, sizeof(sequenceNumber));

    auto plaintext = player.getCipher().decrypt(message, aad);
    player.incrementSequenceNumber();

    return plaintext;
}

void Handler::failSafeSendErrorInCleartext(const TcpSocket &socket, const InfoMessage &message) {
    try {
        socket.send(message.serialize());
    } catch (const std::exception &exception) {
        std::cout << "Impossible to send the error message. " << exception.what() << std::endl;
    }
}

void Handler::failSafeSendErrorInCiphertext(const TcpSocket &socket,
                                            Player &player,
                                            const InfoMessage &message,
                                            PlayerRemovalList &removalList) {
    try {
        auto authenticatedCiphertext = encryptAndAuthenticate(&message, player);
        socket.send(authenticatedCiphertext);
    } catch (const std::exception &exception) {
        std::cout << "Impossible to send the error message. " << exception.what() << std::endl;
        removalList.insert(player.getUsername());
    }
}

std::pair<const TcpSocket, Player>& Handler::findPlayerByUsername(PlayerList &playerList, const std::string &username) {
    for (auto &iterator : playerList) {
        if (iterator.second.getUsername() == username) {
            return iterator;
        }
    }

    throw std::runtime_error("Player not found");
}

void Handler::setMatchmakingStatus(Player &player,
                                   PlayerStatusList &statusList,
                                   const std::string &matchmakingPlayer,
                                   bool matchmakingInitiator) {
    player.setStatus(Player::Status::MATCHMAKING);
    statusList[player.getUsername()] = Player::Status::MATCHMAKING;
    player.setMatchmakingPlayer(matchmakingPlayer);
    player.setAsMatchmakingInitiator(matchmakingInitiator);
}

void Handler::cancelMatchmakingStatus(Player &player, PlayerStatusList &statusList) {
    player.setStatus(Player::Status::MATCHMAKING_INTERRUPTED);
    statusList[player.getUsername()] = Player::Status::MATCHMAKING_INTERRUPTED;
    player.setMatchmakingPlayer("");
    player.setAsMatchmakingInitiator(false);
}

}
