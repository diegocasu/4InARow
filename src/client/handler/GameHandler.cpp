#include <iostream>
#include <limits>
#include <Constants.h>
#include <Utils.h>
#include <SerializationException.h>
#include <FourInARow.h>
#include <InfoMessage.h>
#include "GameHandler.h"

namespace fourinarow {

void GameHandler::clearStdin() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void GameHandler::clearScreen() {
    std::cout << "\033[2J\033[1;1H" << std::flush;
}

void GameHandler::printAvailableCommands() {
    std::cout << "It's your turn! What do you want to do?\n";
    std::cout << " 1) Make a move\n 2) Leave the match\n";
    std::cout << "You have " << MAX_TURN_DURATION << " seconds to make a move!\n";
    std::cout << "Insert the number corresponding to your choice: " << std::flush;
}

void GameHandler::printMatchResult(const FourInARow::Result &result) {
    if (result == FourInARow::Result::WIN) {
        std::cout << "You win! Those moves were superb!\n";
    } else if (result == FourInARow::Result::LOSS) {
        std::cout << "You lose. Play again to get better! Or search for \"solved games\"...\n";
    } else {
        std::cout << "Draw! What a tight match!\n";
    }
}

bool GameHandler::isMoveCommand(const unsigned int &command) {
    return command == 1;
}

bool GameHandler::isExitCommand(const unsigned int &command) {
    return command == 2;
}

unsigned int GameHandler::parseCommand() {
    auto command = 0u;
    std::cin >> command;

    while (std::cin.fail() || command == 0 || command > 2) {
        std::cout << "Invalid input. Please enter one of the above numbers: " << std::flush;
        clearStdin();
        std::cin >> command;
    }

    clearStdin();
    return command;
}

Move GameHandler::parseMove(FourInARow &gameBoard) {
    std::cout << "Insert a column number between 0 and " << std::to_string(COLUMNS - 1) << ": " << std::flush;
    auto column = 0u;
    std::cin >> column;

    while (std::cin.fail() || column >= COLUMNS || !gameBoard.registerMove(column, false)) {
        std::cout << "Invalid input. Please enter a valid column index: " << std::flush;
        clearStdin();
        std::cin >> column;
    }

    clearStdin();
    return Move(column);
}

bool GameHandler::handleUserTurn(const TcpSocket &socket, Player &opponent, FourInARow &gameBoard) {
    printAvailableCommands();
    auto command = parseCommand();

    if (isExitCommand(command)) {
        InfoMessage goodbyeMessage(GOODBYE);
        socket.send(encryptAndAuthenticate(&goodbyeMessage, opponent));
        std::cout << "Returning to the main menu...\n" << std::endl;
        return false;
    }

    auto moveMessage = parseMove(gameBoard);
    socket.send(encryptAndAuthenticate(&moveMessage, opponent));
    clearScreen();
    std::cout << gameBoard.toString() << std::endl;

    if (gameBoard.isMatchFinished()) {
        printMatchResult(gameBoard.getResult());
        std::cout << "Returning to the main menu...\n" << std::endl;
        return false;
    }

    return true;
}

bool GameHandler::receiveOpponentMove(const TcpSocket &socket, Player &opponent, FourInARow &gameBoard) {
    std::cout << "Waiting for " << gameBoard.getOpponent() << "'s move..." << std::endl;
    auto encryptedMessage = socket.receiveWithTimeout(MAX_TURN_DURATION);

    auto message = authenticateAndDecrypt(encryptedMessage, opponent);
    auto type = getMessageType<SerializationException>(message);

    if (type == GOODBYE) {
        std::cout << gameBoard.getOpponent() << " has left the match. Returning to the main menu...\n" << std::endl;
        return false;
    }

    Move opponentMove;
    opponentMove.deserialize(message);
    cleanse(message);
    cleanse(type);

    if (!gameBoard.registerMove(opponentMove.getColumn(), true)) {
        std::cout << gameBoard.getOpponent() << " is trying to cheat. What a loser! Closing the communication...\n" << std::endl;
        return false;
    }

    clearScreen();
    std::cout << gameBoard.toString() << std::endl;

    if (gameBoard.isMatchFinished()) {
        printMatchResult(gameBoard.getResult());
        std::cout << "Returning to the main menu...\n" << std::endl;
        return false;
    }

    return true;
}

void GameHandler::handle(const TcpSocket &socket,
                         Player &opponent,
                         const std::string &opponentUsername,
                         bool firstToPlay) {
    try {
        FourInARow gameBoard(opponentUsername);
        clearScreen();
        std::cout << gameBoard.toString() << std::endl;

        while (true) {
            if (firstToPlay) {
                if (!handleUserTurn(socket, opponent, gameBoard)) {
                    return;
                }
                if (!receiveOpponentMove(socket, opponent, gameBoard)) {
                    return;
                }
            } else {
                if (!receiveOpponentMove(socket, opponent, gameBoard)) {
                    return;
                }
                if (!handleUserTurn(socket, opponent, gameBoard)) {
                    return;
                }
            }
        }
    } catch (const std::exception &exception) {
        std::cerr << "\nCommunication error. " << exception.what() << std::endl;
        std::cerr << "Returning to the main menu...\n" << std::endl;
    }
}

void GameHandler::sendEndGame(const TcpSocket &serverSocket, Player &myselfForServer) {
    try {
        InfoMessage endGame(END_GAME);
        serverSocket.send(encryptAndAuthenticate(&endGame, myselfForServer));
    } catch (const std::exception &exception) {
        std::cerr << "Cannot notify the server about the end of the match. " << exception.what() << std::endl;
        throw std::runtime_error("Cannot notify end game");
    }

}

}
