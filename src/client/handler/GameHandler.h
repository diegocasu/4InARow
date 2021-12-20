#ifndef INC_4INAROW_GAMEHANDLER_H
#define INC_4INAROW_GAMEHANDLER_H

#include <Move.h>
#include "Handler.h"

namespace fourinarow {

/**
 * Class representing a handler for P2P matches between players.
 */
class GameHandler : public Handler {
    private:

        /**
         * Clears <code>stdin</code> by resetting the stream state and flushing the buffer.
         */
        static void clearStdin();

        /**
         * Clears the screen.
         */
        static void clearScreen();

        /**
         * Prints the result of the match in a human readable way.
         * @param result  the result of the match.
         */
        static void printMatchResult(const FourInARow::Result &result);

        /**
         * Prints the available commands.
         */
        static void printAvailableCommands();

        /**
         * Checks if the given command is a move one.
         * @param command  the command.
         * @return         true if the user asked to make a move, false otherwise.
         */
        static bool isMoveCommand(const unsigned int &command);

        /**
         * Checks if the given command is an exit one.
         * @param command  the command.
         * @return         true if the user asked to exit the application, false otherwise.
         */
        static bool isExitCommand(const unsigned int &command);

        /**
         * Parses a command of the user. The method does not return
         * until the supplied command is a valid one.
         * @return  the command inserted by the user.
         */
        static unsigned int parseCommand();

        /**
         * Parses a move of the user, updating the game board.
         * The method does not return until a valid column has been supplied.
         * @param gameBoard  the game board.
         * @return           the <code>MOVE</code> message encapsulating the move.
         */
        static Move parseMove(FourInARow &gameBoard);

        /**
         * Handles the turn of the user, parsing and managing the commands.
         * @param socket     the socket used to communicate with the opponent.
         * @param opponent   the object storing the quantities derived in the handshake with the player.
         * @param gameBoard  the game board.
         * @return           true if the match has finished or the user wants to leave the match,
         *                   false otherwise.
         * @throws SocketException         if an error occurred while sending the move to the opponent.
         * @throws SerializationException  if an error occurred while serializing the move.
         * @throws CryptoException         if an error occurred while encrypting the move, or the maximum
         *                                 sequence number has been reached.
         */
        static bool handleUserTurn(const TcpSocket &socket, Player &opponent, FourInARow &gameBoard);

        /**
         * Handles the turn of the opponent, waiting for the reception of a <code>MOVE/</code> message.
         * @param socket     the socket used to communicate with the opponent.
         * @param opponent   the object storing the quantities derived in the handshake with the player.
         * @param gameBoard  the game board.
         * @return           true if the move was valid and the match has not ended, false otherwise.
         * @throws SocketException         if an error occurred while receiving the move from the opponent.
         * @throws SerializationException  if an error occurred while deserializing the received message.
         * @throws CryptoException         if an error occurred while decrypting the message, or the maximum
         *                                 sequence number has been reached.
         */
        static bool receiveOpponentMove(const TcpSocket &socket, Player &opponent, FourInARow &gameBoard);
    public:
        GameHandler() = delete;
        ~GameHandler() = delete;
        GameHandler(const GameHandler&) = delete;
        GameHandler(GameHandler&&) = delete;
        GameHandler& operator=(const GameHandler&) = delete;
        GameHandler& operator=(GameHandler&&) = delete;

        /**
         * Handles a P2P game with another player.
         * @param socket            the socket used to communicate with the opponent.
         * @param opponent          the object storing the quantities derived in the handshake with the player.
         * @param opponentUsername  the username of the opponent.
         * @param firstToPlay       true if the user has the first turn, false otherwise.
         */
        static void handle(const TcpSocket &socket,
                           Player &opponent,
                           const std::string &opponentUsername,
                           bool firstToPlay);

        /**
         * Sens a <code>END_GAME</code> message to the server to notify
         * that the P2P match has ended and the player is available.
         * @param serverSocket     the socket used to communicate with the server.
         * @param myselfForServer  the object storing the quantities derived in the handshake with the server.
         * @throws runtime_error  if an error occurred while sending the message to the server.
         */
        static void sendEndGame(const TcpSocket &serverSocket, Player &myselfForServer);
};

}


#endif //INC_4INAROW_GAMEHANDLER_H
