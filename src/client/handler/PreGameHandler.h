#ifndef INC_4INAROW_PREGAMEHANDLER_H
#define INC_4INAROW_PREGAMEHANDLER_H

#include <string>
#include <TcpSocket.h>
#include <InputMultiplexer.h>
#include <Challenge.h>
#include <PlayerMessage.h>
#include "Handler.h"

namespace fourinarow {

/**
 * Class representing a handler for the commands inserted by the user before a game.
 * It exploits the <code>CLI</code> to print the list of available commands and parse the user inputs.
 * It enables the user to refresh the player list, send a challenge to another player or
 * exit the application.
 */
class PreGameHandler : Handler {
    private:
        /**
         * Checks if the given command is a player list refresh.
         * @param command  the command.
         * @return         true if a refresh of the player list is asked, false otherwise.
         */
        static bool isRefreshPlayerListCommand(const unsigned int &command);

        /**
         * Checks if the given command is an exit one.
         * @param command     the command.
         * @param playerList  the player list.
         * @return            true if the user asks to exit the application, false otherwise.
         */
        static bool isExitCommand(const unsigned int &command, const std::string &playerList);

        /**
         * Checks if the given command is a challenge one.
         * @param command     the command.
         * @param playerList  the player list.
         * @return            true if the user asks to send a challenge, false otherwise.
         */
        static bool isChallengeCommand(const unsigned int &command, const std::string &playerList);

        /**
         * Checks if the given message is a response to a <code>CHALLENGE</code> message,
         * namely if it has type <code>PLAYER_NOT_AVAILABLE, CHALLENGE_REFUSED</code>
         * or <code>CHALLENGE_ACCEPTED</code>;
         * @param type  the message type.
         * @return      true if the message is a response to a <code>CHALLENGE</code> message,
         *              false otherwise.
         */
        static bool isChallengeResponseMessage(const uint8_t &type);

        /**
         * Clears <code>stdin</code> by resetting the stream state and flushing the buffer.
         */
        static void clearStdin();

        /**
         * Prints the formatted player list.
         * @param playerList  the player list.
         */
        static void printPlayerList(const std::string &playerList);

        /**
         * Prints the available commands.
         * The set of commands depends on the player list being empty or not.
         * @param playerList  the player list.
         */
        static void printAvailableCommands(const std::string &playerList);

        /**
         * Prints a challenge request received from a user.
         * @param username  the username of the opponent.
         */
        static void printChallengeRequest(const std::string &username);

        /**
         * Parses a command of the user, storing it in the given variable.
         * It performs only one trial, returning true if the attempt is successful.
         * @param playerList     the player list.
         * @param outputCommand  the reference to the variable that will store the command.
         *                       It is modified only if the parsing is successful.
         * @return               true if the parsing is successful, false otherwise.
         */
        static bool parseCommand(const std::string &playerList, unsigned int &outputCommand);

        /**
         * Parses the username of the player to challenge. The method
         * does not return until a valid username has been supplied.
         * @param playerList  the player list.
         * @return            the username of the player to challenge.
         */
        static std::string parseOpponentUsername(const std::string &playerList);

        /**
         * Parses the user answer to an incoming challenge request.
         * The method does not return until a valid answer has been supplied.
         * @return  true if the challenge is accepted, false otherwise.
         */
        static bool parseChallengeRequestAnswer();

        /**
         * Handles the response of the user to an incoming challenge request.
         * It parses the response of the user and sends it to the server.
         * It does not wait for the reception of a <code>PLAYER</code> message.
         * @param socket            the socket used to communicate with the server.
         * @param challenge         the object storing the received <code>CHALLENGE</code> request.
         * @param myselfForServer   the object storing the quantities needed to communicate with the server.
         * @param playerList        the player list.
         * @return                  true if the user accepts the challenge request, false otherwise.
         * @throws SocketException  if an error occurs while sending the response,
         *                          or the remote socket has been closed.
         * @throws CryptoException  if an error occurs while encrypting the response, or
         *                          the maximum sequence number has been reached.
         */
        static bool handleResponseToIncomingChallenge(const TcpSocket &socket,
                                                      const Challenge &challenge,
                                                      Player &myselfForServer,
                                                      const std::string &playerList);

        /**
         * Handles an incoming message sent by the server not in response to a user command.
         * If the message is a <code>CHALLENGE</code one, it prompts the user to accept or refuse the request.
         * @param socket            the socket used to communicate with the server.
         * @param multiplexer       the multiplexer handling <code>stdin</code> and the socket.
         * @param myselfForServer   the object storing the quantities needed to communicate with the server.
         * @param playerList        the player list.
         * @param opponent          a reference to an empty object that will store the <code>PLAYER</code> message
         *                          containing the information about the opponent. The content of this object
         *                          is valid only if a challenge is accepted, i.e. if the method returns true.
         * @param opponentUsername  a reference to an empty string that will store the username of the opponent.
         *                          The content of the string is valid only if a challenge is accepted,
         *                          i.e. if the method returns true.
         * @return                  true if the message is a challenge request, the user accepts it
         *                          and a <code>PLAYER</code> message is received correctly, false otherwise.
         * @throws runtime_error  if an error occurs while handling the incoming message.
         */
        static bool handleIncomingMessage(const TcpSocket &socket,
                                          const InputMultiplexer &multiplexer,
                                          Player &myselfForServer,
                                          const std::string &playerList,
                                          PlayerMessage &opponent,
                                          std::string &opponentUsername);

        /**
         * Handles a command for refreshing the player list. If a pending <code>CHALLENGE</code> message
         * is waiting to be processed, the method does not perform the refresh,
         * but handles the <code>CHALLENGE</code>.
         * @param socket             the socket used to communicate with the server.
         * @param myselfForServer    the object storing the quantities needed to communicate with the server.
         * @param currentPlayerList  the current player list. It is modified only if there is no pending challenge
         *                           request and the refresh succeeds.
         * @param opponent           a reference to an empty object that will store the <code>PLAYER</code> message
         *                           containing the information about the opponent. The content of this object
         *                           is valid only if a pending challenge is accepted, i.e. if the method returns true.
         * @param opponentUsername   a reference to an empty string that will store the username of the opponent.
         *                           The content of the string is valid only if a pending challenge is accepted,
         *                           i.e. if the method returns true.
         * @return                   true if a pending <code>CHALLENGE<code> message is present, the user accepts it
         *                           and a <code>PLAYER</code> message is received correctly, false otherwise.
         * @throws runtime_error  if an error occurs while refreshing the player list or
         *                        managing a pending challenge request.
         */
        static bool handlePlayerListRefreshCommand(const TcpSocket &socket,
                                                   Player &myselfForServer,
                                                   std::string &currentPlayerList,
                                                   PlayerMessage &opponent,
                                                   std::string &opponentUsername);
        /**
         * Handles a command for exiting the application.
         * @param socket            the socket used to communicate with the server.
         * @param myselfForServer   the object storing the quantities needed to communicate with the server.
         * @throws runtime_error  if an error occurs while sending the application exit message.
         */
        static void handleExitCommand(const TcpSocket &socket, Player &myselfForServer);

        /**
         * Waits for the reception of a matchmaking response message.
         * If a response does not arrive before <code>CLIENT_MATCHMAKING_TIMEOUT</code> seconds,
         * the matchmaking is aborted and an empty vector is returned.
         * @param socket           the socket used to communicate with the server.
         * @param myselfForServer  the object storing the quantities needed to communicate with the server.
         * @return                 an empty vector if the matchmaking is aborted, an encrypted message
         *                         if the reception succeeds.
         * @throws SocketException  if an error occurs while aborting the matchmaking,
         *                          or the remote socket has been closed.
         * @throws CryptoException  if the maximum sequence number has been reached.
         */
        static std::vector<unsigned char> receiveMessageOrCancelMatchmaking(const TcpSocket &socket,
                                                                            Player &myselfForServer);

        /**
         * Receives a response to a challenge sent by this client.
         * If a pending <code>CHALLENGE</code> message is present, the challenge sent by this client
         * is ignored and the method handles the pending one.
         * If no message is received, the matchmaking is aborted.
         * Note that the method does not implement the reception of a <code>PLAYER</code> message
         * when a pending challenge is found.
         * @param socket            the socket used to communicate with the server.
         * @param myselfForServer   the object storing the quantities needed to communicate with the server.
         * @param playerList        the player list.
         * @return                  true if a <code>CHALLENGE_ACCEPTED</code> message is received,
         *                          or a pending <code>CHALLENGE</code> request is accepted;
         *                          false otherwise.
         * @throws SocketException         if an error occurs while aborting the matchmaking,
         *                                 or the remote socket has been closed.
         * @throws SerializationException  if an error occurs while deserializing the message.
         * @throws CryptoException         if an error occurs while decrypting the message, or
         *                                 the maximum sequence number has been reached.
         */
        static bool receiveChallengeResponse(const TcpSocket &socket,
                                             Player &myselfForServer,
                                             const std::string &playerList);

        /**
         * Receives a <code>PLAYER</code> message, concluding the matchmaking.
         * If no message is received, the matchmaking is aborted.
         * @param socket           the socket used to communicate with the server.
         * @param myselfForServer  the object storing the quantities needed to communicate with the server.
         * @param playerList       the player list.
         * @param opponent         a reference to an empty object that will store the <code>PLAYER</code> message.
         *                         The content of this object is valid only if a <code>PLAYER</code> message
         *                         is received, i.e. if the method returns true.
         * @return                 true if a <code>PLAYER</code> message is received, false otherwise.
         * @throws SocketException         if an error occurs while aborting the matchmaking,
         *                                 or the remote socket has been closed.
         * @throws SerializationException  if an error occurs while deserializing the message.
         * @throws CryptoException         if an error occurs while decrypting the message, or
         *                                 the maximum sequence number has been reached.
         */
        static bool receivePlayerMessage(const TcpSocket &socket,
                                         Player &myselfForServer,
                                         const std::string &playerList,
                                         PlayerMessage &opponent);

        /**
         * Handles a command for challenging another player.
         * @param socket            the socket used to communicate with the server.
         * @param myselfForServer   the object storing the quantities needed to communicate with the server.
         * @param playerList        the player list.
         * @param opponent          a reference to an empty object that will store the <code>PLAYER</code> message
         *                          containing the information about the opponent. The content of this object
         *                          is valid only if a challenge is accepted, i.e. if the method returns true.
         * @param opponentUsername  a reference to an empty string that will store the username of the opponent.
         *                          The content of the string is valid only if a challenge is accepted,
         *                          i.e. if the method returns true.
         * @return                  true if the matchmaking succeeds, false otherwise.
         * @throws runtime_error  if an error occurs while challenging another player.
         */
        static bool handleChallengeCommand(const TcpSocket &socket,
                                           Player &myselfForServer,
                                           const std::string &playerList,
                                           PlayerMessage &opponent,
                                           std::string &opponentUsername);
    public:
        PreGameHandler() = delete;
        ~PreGameHandler() = delete;
        PreGameHandler(const PreGameHandler&) = delete;
        PreGameHandler(PreGameHandler&&) = delete;
        PreGameHandler& operator=(const PreGameHandler&) = delete;
        PreGameHandler& operator=(PreGameHandler&&) = delete;

        /**
         * Handles the pre-game phase, accepting the commands inserted by
         * the user via <code>CLI</code>, or managing incoming challenge requests.
         * It returns only if:
         * 1) the user wants to exit the application;
         * 2) a P2P match must be set up.
         * This method does not handle the P2P handshake between players.
         * @param socket           the socket used to communicate with the server.
         * @param myselfForServer  the object storing the quantities needed to communicate with the server.
         * @param firstPlayerList  the initial player list.
         * @param opponent         a reference to an empty object that will store the <code>PLAYER</code> message
         *                         containing the information about the opponent. The content of this object
         *                         is valid only if a challenge is accepted, i.e. if the method returns true.
         * @param opponentUsername  a reference to an empty string that will store the username of the opponent.
         *                          The content of the string is valid only if a challenge is accepted,
         *                          i.e. if the method returns true.
         * @return                 true if a match must be set up, false if the user wants to exit the application.
         */
        static bool handle(const TcpSocket &socket,
                           Player &myselfForServer,
                           const std::string &firstPlayerList,
                           PlayerMessage &opponent,
                           std::string &opponentUsername);
};

}

#endif //INC_4INAROW_PREGAMEHANDLER_H
