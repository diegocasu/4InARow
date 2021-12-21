#ifndef INC_4INAROW_HANDLER_H
#define INC_4INAROW_HANDLER_H

#include <unordered_map>
#include <unordered_set>
#include <TcpSocket.h>
#include <Player.h>
#include <TcpSocketHasher.h>
#include <InfoMessage.h>

namespace fourinarow {

/**
 * Base class for server handlers. It is used to store shared methods.
 */
class Handler {
    protected:
        using PlayerList = std::unordered_map<TcpSocket, Player, TcpSocketHasher>;
        using PlayerStatusList = std::unordered_map<std::string, Player::Status>;
        using PlayerRemovalList = std::unordered_set<std::string>;

        /**
         * Generates a string containing the list of players in the <code>AVAILABLE</code> status.
         * The string has format <code>"PLAYER1;PLAYER2;....;PLAYERn"</code>.
         * @param statusList        the player status list.
         * @param excludedUsername  the username of the player that will receive the list.
         * @return                  the list of available players. It can be empty.
         */
        static std::string generatePlayerList(const PlayerStatusList &statusList, const std::string &excludedUsername);

        /**
         * Performs the authenticated encryption of the given message,
         * returning the ciphertext concatenated with the tag.
         * @param message  the message to encrypt and authenticate.
         * @param player   the player to which the message will be sent.
         * @return         the ciphertext concatenated with the tag.
         * @throws SerializationException  if the message has not the expected format
         * @throws CryptoException         if an error occurs while encrypting the message,
         *                                 or the maximum sequence number has been reached.
         */
        static std::vector<unsigned char> encryptAndAuthenticate(const Message *message, Player &player);

        /**
         * Performs the authenticated decryption of the given message, returning the plaintext.
         * @param message  the encrypted message.
         * @param player   the player receiving the message.
         * @return         the decrypted message.
         * @throws CryptoException  if an error occurs while decrypting the message,
         *                          or the tag is not valid,
         *                          or the maximum sequence number has been reached.
         */
        static std::vector<unsigned char> authenticateAndDecrypt(std::vector<unsigned char> &message, Player &player);

        /**
         * Sends an error message in cleartext through the given socket,
         * without throwing an exception if a failure occurs.
         * This method is useful for sending error messages inside a <code>catch</code> clause.
         * Note that no removal list is passed, because messages are exchanged
         * in cleartext only during the handshake: a failure in such phase
         * always causes a disconnection of the client.
         * @param socket       the socket used to communicate with the player.
         * @param message      the error message.
         */
        static void failSafeSendErrorInCleartext(const TcpSocket &socket, const InfoMessage &message);

        /**
         * Sends an error message using authenticated encryption through the given socket,
         * without throwing an exception if a failure occurs.
         * If the maximum sequence number has been reached or the socket has been closed,
         * the player is put in the removal list.
         * This method is useful for sending error messages inside a <code>catch</code> clause.
         * @param socket       the socket used to communicate with the player.
         * @param player       the player.
         * @param message      the error message.
         * @param removalList  the player removal list.
         */
        static void failSafeSendErrorInCiphertext(const TcpSocket &socket,
                                                  Player &player,
                                                  const InfoMessage &message,
                                                  PlayerRemovalList &removalList);

        /**
         * Finds a player and returns the corresponding entry in the player list.
         * @param playerList  the player list.
         * @param username    the username of the player.
         * @return            the reference to the player's entry in the player list.
         * @throws runtime_error  if the player is not in the player list.
         */
        static std::pair<const TcpSocket, Player>& findPlayerByUsername(PlayerList &playerList,
                                                                        const std::string &username);

        /**
         * Changes the status of a player to <code>MATCHMAKING</code>.
         * @param player                the player.
         * @param statusList            the player status list.
         * @param matchmakingPlayer     the other player involved in the matchmaking.
         * @param matchmakingInitiator  true if the player is the initiator of the matchmaking,
         *                              false otherwise.
         */
        static void setMatchmakingStatus(Player &player,
                                         PlayerStatusList &statusList,
                                         const std::string &matchmakingPlayer,
                                         bool matchmakingInitiator);
        /**
         * Puts a <code>MATCHMAKING</code> player in the <code>MATCHMAKING_INTERRUPTED</code> state.
         * @param player      the player.
         * @param statusList  the player status list.
         */
        static void cancelMatchmakingStatus(Player &player, PlayerStatusList &statusList);
    public:
        Handler() = delete;
        ~Handler() = delete;
        Handler(const Handler&) = delete;
        Handler(Handler&&) = delete;
        Handler& operator=(const Handler&) = delete;
        Handler& operator=(Handler&&) = delete;
};

}

#endif //INC_4INAROW_HANDLER_H
