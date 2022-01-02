#include <iostream>
#include <limits>
#include <sstream>
#include <set>
#include <unistd.h>
#include <Constants.h>
#include <Utils.h>
#include <SerializationException.h>
#include <SocketException.h>
#include <CryptoException.h>
#include <InfoMessage.h>
#include <PlayerListMessage.h>
#include "PreGameHandler.h"

namespace fourinarow {

bool PreGameHandler::isRefreshPlayerListCommand(const unsigned int &command) {
    return command == 1;
}

bool PreGameHandler::isExitCommand(const unsigned int &command, const std::string &playerList) {
    return (playerList.empty() && command == 2) || (!playerList.empty() && command == 3);
}

bool PreGameHandler::isChallengeCommand(const unsigned int &command, const std::string &playerList) {
    return !playerList.empty() && command == 2;
}

bool PreGameHandler::isChallengeResponseMessage(const uint8_t &type) {
    return type == PLAYER_NOT_AVAILABLE
           || type == CHALLENGE_REFUSED
           || type == CHALLENGE_ACCEPTED;
}

void PreGameHandler::clearStdin() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void PreGameHandler::printPlayerList(const std::string &playerList) {
    std::cout << "\n--------------- PLAYER LIST ---------------\n";

    if (playerList.empty()) {
        std::cout << "No players available at the moment!\n";
    } else {
        std::istringstream playerStream(playerList);
        std::string player;

        while (std::getline(playerStream, player, ';')) {
            std::cout << "\u25CF" << ' ' << player << '\n';
        }
    }

    std::cout << "-------------------------------------------" << std::endl;
}

void PreGameHandler::printAvailableCommands(const std::string &playerList) {
    std::cout << "What do you want to do?" << std::endl;

    if (playerList.empty()) {
        std::cout << " 1) Refresh the player list\n 2) Exit the application\n";
    } else {
        std::cout << " 1) Refresh the player list\n 2) Challenge a user\n 3) Exit the application\n";
    }

    std::cout << "Insert the number corresponding to your choice: " << std::flush;
}

void PreGameHandler::printChallengeRequest(const std::string &username) {
    std::cout << "\nYou have received a challenge request from '" << username + "'. ";
    std::cout << "Do you want to play? [y/n]: " << std::flush;
}

bool PreGameHandler::parseCommand(const std::string &playerList, unsigned int &outputCommand) {
    auto command = 0u;
    std::cin >> command;

    if (std::cin.fail() || command == 0 || (playerList.empty() && command > 2) || (!playerList.empty() && command > 3)) {
        clearStdin();
        std::cout << "Invalid input. Please enter one of the above numbers: " << std::flush;
        return false;
    }

    outputCommand = command;
    clearStdin();
    return true;
}

std::string PreGameHandler::parseOpponentUsername(const std::string &playerList) {
    // Extract the available players from the current player list.
    std::set<std::string> players;
    std::istringstream stream(playerList);
    std::string player;
    while (std::getline(stream, player, ';')) {
        players.insert(player);
    }

    std::cout << "Insert the username of the player: " << std::flush;
    std::string username;
    std::cin >> username; // A username cannot have whitespaces, so getline() is not needed.

    while (std::cin.fail() || players.count(username) == 0) {
        std::cout << "This username is not in the list of players. Please enter the username again: " << std::flush;
        clearStdin();
        std::cin >> username;
    }

    clearStdin();
    return username;
}

bool PreGameHandler::parseChallengeRequestAnswer() {
    std::string answer;
    std::cin >> answer;

    while (std::cin.fail() || (answer != "y" && answer != "yes" && answer != "n" && answer != "no")) {
        std::cout << "Invalid answer. Please type 'yes' or 'no': " << std::flush;
        clearStdin();
        std::cin >> answer;
    }
    clearStdin();

    if (answer == "y" || answer == "yes") {
        return true;
    }
    return false;
}

bool PreGameHandler::handleIncomingMessage(const TcpSocket &socket,
                                           const InputMultiplexer &multiplexer,
                                           Player &myselfForServer,
                                           const std::string &playerList,
                                           PlayerMessage &opponent,
                                           std::string &opponentUsername) {
    // Clear pending input data, if any.
    if (multiplexer.isReady(STDIN_FILENO)) {
        clearStdin();
    }

    try {
        auto encryptedMessage = socket.receive();
        auto message = authenticateAndDecrypt(encryptedMessage, myselfForServer);
        auto type = getMessageType<SerializationException>(message);

        if (type != CHALLENGE) {
            // Ignore the message. It could be a spurious message due to a failed matchmaking.
            cleanse(message);
            cleanse(type);
            return false;
        }

        Challenge challenge;
        challenge.deserialize(message);
        cleanse(message);
        cleanse(type);

        printChallengeRequest(challenge.getUsername());
        auto challengeAccepted = parseChallengeRequestAnswer();

        if (challengeAccepted) {
            InfoMessage response(CHALLENGE_ACCEPTED);
            socket.send(encryptAndAuthenticate(&response, myselfForServer));

            if (!receivePlayerMessage(socket, myselfForServer, playerList, opponent)) {
                return false;
            }

            opponentUsername = std::move(challenge.getUsername());
            return true;
        }

        InfoMessage response(CHALLENGE_REFUSED);
        socket.send(encryptAndAuthenticate(&response, myselfForServer));
        std::cout << '\n';
        printAvailableCommands(playerList);
        return false;
    } catch (const std::exception &exception) {
        std::cerr << "\nCommunication error. " << exception.what() << std::endl;
        throw std::runtime_error("Lost connection with the server");
    }
}

void PreGameHandler::handlePlayerListRefreshCommand(const TcpSocket &socket,
                                                    Player &myselfForServer,
                                                    std::string &currentPlayerList) {
    try {
        InfoMessage requestPlayerList(REQ_PLAYER_LIST);
        socket.send(encryptAndAuthenticate(&requestPlayerList, myselfForServer));

        auto encryptedMessage = socket.receiveWithTimeout(CLIENT_PROTOCOL_TIMEOUT);
        auto message = authenticateAndDecrypt(encryptedMessage, myselfForServer);
        auto type = getMessageType<SerializationException>(message);

        if (type != PLAYER_LIST) {
            std::cout << "An error occurred while synchronizing the player list. Try again.\n" << std::endl;
            printAvailableCommands(currentPlayerList);
            cleanse(message);
            cleanse(type);
            return;
        }

        PlayerListMessage playerList;
        playerList.deserialize(message);
        cleanse(message);
        cleanse(type);

        currentPlayerList = playerList.getPlayerList();
        printPlayerList(currentPlayerList);
        printAvailableCommands(currentPlayerList);
    } catch(const CryptoException &exception) {
        /* In case of a pending incoming challenge, arrived while the user was writing
         * the username of the opponent, the sequence number of the client could be
         * temporarily desynchronized from the one of the server, causing a tag mismatch.
         * In this case, the matchmaking is cancelled for both players and the request is ignored.
         * The sequence number is synchronized again starting from the next message.
         */
        std::cout << "An error occurred while synchronizing the player list. Try again.\n" << std::endl;
        printAvailableCommands(currentPlayerList);

    } catch (const std::exception &exception) {
        std::cerr << "Impossible to handle the player list refresh. ";
        std::cerr << exception.what() << std::endl;
        throw std::runtime_error("Cannot handle the player list refresh");
    }
}

void PreGameHandler::handleExitCommand(const TcpSocket &socket, Player &myselfForServer) {
    try {
        InfoMessage goodbye(GOODBYE);
        socket.send(encryptAndAuthenticate(&goodbye, myselfForServer));
    } catch (const std::exception &exception) {
        std::cerr << "Impossible to notify the server about the application exit. " << exception.what() << std::endl;
        throw std::runtime_error("Cannot notify the server about exit");
    }
}

std::vector<unsigned char> PreGameHandler::receiveMessageOrCancelMatchmaking(const TcpSocket &socket,
                                                                             Player &myselfForServer) {
    try {
        return socket.receiveWithTimeout(CLIENT_MATCHMAKING_TIMEOUT);
    } catch (const SocketException &exception) {
        /*
         * If the timeout has expired, the other player is not responding and is causing
         * this client to hang. Then, the matchmaking is aborted by sending a PLAYER_NOT_AVAILABLE
         * message to the server: since this type of message is not expected at this point
         * of the matchmaking, the server will manage it as an error in the protocol and
         * cancel the matchmaking, forcing both clients in the MATCHMAKING_INTERRUPTED state.
         * Achieved this result, the client can accept again commands from the user.
         * If the error is due to the remote server socket being closed, the following send()
         * will throw and the resulting exception will be caught in the caller,
         * causing the client to shut down as expected.
         */
        InfoMessage abortMatchmaking(PLAYER_NOT_AVAILABLE);
        socket.send(encryptAndAuthenticate(&abortMatchmaking, myselfForServer));
        return std::vector<unsigned char>();
    }
}

bool PreGameHandler::receiveChallengeResponse(const TcpSocket &socket,
                                              Player &myselfForServer,
                                              const std::string &playerList) {
    std::cout << "Challenge sent. Waiting for a response from the other player..." << std::endl;
    auto challengeResponseMessage = receiveMessageOrCancelMatchmaking(socket, myselfForServer);

    if (challengeResponseMessage.empty()) {
        std::cout << "Matchmaking failed. Try to refresh the player list\n" << std::endl;
        printAvailableCommands(playerList);
        return false;
    }

    auto message = authenticateAndDecrypt(challengeResponseMessage, myselfForServer);
    auto type = getMessageType<SerializationException>(message);
    cleanse(message);

    if (!isChallengeResponseMessage(type) || type == PLAYER_NOT_AVAILABLE) {
        std::cout << "Matchmaking failed. Try to refresh the player list\n" << std::endl;
        printAvailableCommands(playerList);
        cleanse(type);
        return false;
    }

    if (type == CHALLENGE_REFUSED) {
        std::cout << "The user has refused your challenge\n" << std::endl;
        printAvailableCommands(playerList);
        cleanse(type);
        return false;
    }

    std::cout << "The user has accepted the challenge!" << std::endl;
    cleanse(type);
    return true;
}

bool PreGameHandler::receivePlayerMessage(const TcpSocket &socket,
                                          Player &myselfForServer,
                                          const std::string &playerList,
                                          PlayerMessage &opponent) {
    std::cout << "Receiving the player profile..." << std::endl;
    auto playerMessage = receiveMessageOrCancelMatchmaking(socket, myselfForServer);

    if (playerMessage.empty()) {
        std::cout << "Matchmaking failed. Try to refresh the player list\n" << std::endl;
        printAvailableCommands(playerList);
        return false;
    }

    auto message = authenticateAndDecrypt(playerMessage, myselfForServer);
    auto type = getMessageType<SerializationException>(message);

    if (type != PLAYER) {
        std::cout << "Matchmaking failed. Try to refresh the player list\n" << std::endl;
        printAvailableCommands(playerList);
        cleanse(message);
        cleanse(type);
        return false;
    }

    opponent.deserialize(message);
    cleanse(message);
    cleanse(type);
    return true;
}


bool PreGameHandler::handleChallengeCommand(const TcpSocket &socket,
                                            Player &myselfForServer,
                                            const std::string &playerList,
                                            PlayerMessage &opponent,
                                            std::string &opponentUsername) {
    try {
        auto username = parseOpponentUsername(playerList);
        Challenge challenge(username);
        socket.send(encryptAndAuthenticate(&challenge, myselfForServer));

        if (!receiveChallengeResponse(socket, myselfForServer, playerList)) {
            return false;
        }

        if (!receivePlayerMessage(socket, myselfForServer, playerList, opponent)) {
            return false;
        }

        opponentUsername = std::move(username);
        return true;
    } catch(const CryptoException &exception) {
        /*
         * Suppose the client and the server have sequence number N at the beginning of this method, and
         * suppose a challenge request arrives while the user is writing the username of the opponent.
         * In this case, the pending challenge request has tag generated using N as sequence number,
         * and the server has moved to sequence number N+1. When the user finishes typing, the client
         * sends its CHALLENGE message to the server, generating a tag with sequence number N and moving
         * to sequence number N+1. This means that both the client and the server get a tag mismatch
         * when parsing the pending messages, since both try to verify a tag generated with sequence number
         * N using the current sequence number N+1. In this case, the matchmaking fails and the error must
         * be caught here to avoid a client disconnection. In any case, after the tag mismatch, the sequence
         * numbers are again synchronized on the value N+1, so the communication can continue: the client
         * will receive a MALFORMED_MESSAGE due to the tag mismatch caught by the server, and both will
         * move to the sequence number N+2.
         */
        std::cout << "Matchmaking failed. Try to refresh the player list\n" << std::endl;
        printAvailableCommands(playerList);
        return false;
    } catch (const std::exception &exception) {
        std::cerr << "Impossible to send the challenge. " << exception.what() << std::endl;
        throw std::runtime_error("Cannot send the challenge");
    }
}

bool PreGameHandler::handle(const TcpSocket &socket,
                            Player &myselfForServer,
                            const std::string &firstPlayerList,
                            PlayerMessage &opponent,
                            std::string &opponentUsername) {
    InputMultiplexer multiplexer;
    multiplexer.addDescriptor(socket.getDescriptor());
    multiplexer.addDescriptor(STDIN_FILENO);

    auto currentPlayerList = firstPlayerList;
    printPlayerList(currentPlayerList);
    printAvailableCommands(currentPlayerList);

    while (true) {
        multiplexer.select();

        if (multiplexer.isReady(socket.getDescriptor())) {
            if (handleIncomingMessage(socket, multiplexer, myselfForServer, currentPlayerList, opponent, opponentUsername)) {
                return true;
            }
            continue;
        }

        auto command = 0u;
        if (!parseCommand(currentPlayerList, command)) {
            continue;
        }

        if (isRefreshPlayerListCommand(command)) {
            handlePlayerListRefreshCommand(socket, myselfForServer, currentPlayerList);
            continue;
        }

        if (isExitCommand(command, currentPlayerList)) {
            handleExitCommand(socket, myselfForServer);
            return false;
        }

        if (isChallengeCommand(command, currentPlayerList)) {
            if (handleChallengeCommand(socket, myselfForServer, currentPlayerList, opponent, opponentUsername)) {
                return true;
            }
            continue;
        }
    }
}

}


