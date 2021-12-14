#ifndef INC_4INAROW_PLAYER_H
#define INC_4INAROW_PLAYER_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <TcpSocket.h>
#include <DiffieHellman.h>
#include <AuthenticatedEncryption.h>

namespace fourinarow {

/**
 * Class representing a player. It holds the networking and cryptographic
 * quantities needed to communicate with another party. It can be used by:
 * 1) the server to represent a connecting player, i.e. a client;
 * 2) a player to represent another player in P2P communications;
 * 3) a player to represent herself when communicating with the server, i.e.
 *    to conveniently store quantities on the client side when connecting
 *    to the server.
 * Inside a single Player object, only one party, either the client or the server,
 * can own a generated key pair. Once a party owns a generated pair,
 * the other one is forced to store only a public key.
 */
class Player {
    public:
        enum class Status { OFFLINE, CONNECTED, HANDSHAKE, AVAILABLE, MATCHMAKING, PLAYING };
    private:
        std::string username;
        Status status;
        std::vector<unsigned char> clientNonce;
        std::vector<unsigned char> serverNonce;
        std::unique_ptr<DiffieHellman> clientKeys;
        std::unique_ptr<DiffieHellman> serverKeys;
        std::vector<unsigned char> clientPublicKey;
        std::vector<unsigned char> serverPublicKey;
        std::vector<unsigned char> freshnessProof;
        std::unique_ptr<AuthenticatedEncryption> cipher;
        uint32_t sequenceNumber;
        std::string matchmakingPlayer;

        /**
         * Checks if the nonces, the public key of the other party and
         * the key pair of this party have been initialized.
         * @throws CryptoException  if at least one of the above quantities has not been initialized.
         */
        void checkIfCryptoAttributesInitialized() const;
    public:
        /**
         * Creates a player object, setting its status to <code>OFFLINE</code>.
         */
        Player();

        /**
         * Destroys the object and securely wipes the cryptographic secrets from memory.
         */
        ~Player() = default;

        Player(Player&&) = default;
        Player& operator=(Player&&) = default;
        Player(const Player&) = delete;
        Player& operator=(const Player&) = delete;

        const std::string& getUsername() const;
        Status getStatus() const;
        const std::vector<unsigned char>& getClientNonce() const;
        const std::vector<unsigned char>& getServerNonce() const;
        const std::string& getMatchmakingPlayer() const;
        const std::vector<unsigned char>& getFreshnessProof() const;
        uint32_t getSequenceNumber() const;

        /**
         * Returns the public key of the client. If the key was part of a generated
         * key pair, the calls subsequent to <code>initCipher()</code>
         * return an empty vector, because the method destroys the pair.
         * @return  the public key of the client.
         * @throws SerializationException  if an error occurred while retrieving a generated public key.
         *                                 The exception cannot be thrown if the key was set.
         */
        std::vector<unsigned char> getClientPublicKey() const;

        /**
         * Returns the public key of the server. If the key was part of a generated
         * key pair, the calls subsequent to <code>initCipher()</code>
         * return an empty vector, because the method destroys the pair.
         * @return  the public key of the server.
         * @throws SerializationException  if an error occurred while retrieving a generated public key.
         *                                 The exception cannot be thrown if the key was set.
         */
        const std::vector<unsigned char> getServerPublicKey() const;

        /**
         * Returns the cipher used to encrypt, decrypt and authenticate the communications
         * with the other party. Calls preceding <code>initCipher()</code> are not allowed
         * and throw an exception.
         * @return  the cipher used to communicate with the other party.
         * @throws CryptoException  if the cipher has not been generated yet.
         */
        const AuthenticatedEncryption& getCipher() const;

        void setStatus(Status newStatus);
        void setMatchmakingPlayer(std::string matchmakingPlayer);

        /**
         * Sets the username of the player.
         * @param username  the username.
         * @throws SerializationException  if the username is not a valid one.
         */
        void setUsername(std::string username);

        /**
         * Sets the client nonce.
         * @param nonce  the client nonce.
         * @throws SerializationException  if the nonce is wrongly sized.
         */
        void setClientNonce(std::vector<unsigned char> nonce);

        /**
         * Sets the server nonce.
         * @param nonce  the server nonce.
         * @throws SerializationException  if the nonce is wrongly sized.
         */
        void setServerNonce(std::vector<unsigned char> nonce);

        /**
         * Sets the public key of the client. The key can be set only if
         * a key pair has not been already generated for the client and
         * the public key of the server has not been already set.
         * @param publicKey  the public key of the client.
         * @throws SerializationException  if the public key is wrongly sized.
         * @throws CryptoException         if the key pair of the client has already been generated,
         *                                 or the public key of the server has been already set.
         */
        void setClientPublicKey(std::vector<unsigned char> publicKey);

        /**
         * Sets the public key of the server. The key can be set only if
         * a key pair has not been already generated for the server and
         * the public key of the client has not been already set.
         * @param publicKey  the public key of the server.
         * @throws SerializationException  if the public key is wrongly sized.
         * @throws CryptoException         if the key pair of the server has already been generated,
         *                                 or the public key of the client has been already set.
         */
        void setServerPublicKey(std::vector<unsigned char> publicKey);

        /**
         * Generates and stores a random nonce for the client
         * exploiting <code>RAND_bytes()</code> of OpenSSL.
         * @throws CryptoException  if an error occurred while generating the nonce.
         */
        void generateClientNonce();

        /**
         * Generates and stores a random nonce for the server
         * exploiting <code>RAND_bytes()</code> of OpenSSL.
         * @throws CryptoException  if an error occurred while generating the nonce.
         */
        void generateServerNonce();

        /**
         * Generates and stores a private-public key pair for the client
         * using Elliptic-curve Diffie-Hellman. The pair can be generated only
         * if the key pair of the server has not been already generated and
         * the public key of the client has not been already set.
         * @throws CryptoException  if the public key of the client has already been set,
         *                          or the server key pair has already been generated,
         *                          or an error occurred while generating the pair.
         */
        void generateClientKeys();

        /**
         * Generates and stores a private-public key pair for the server
         * using Elliptic-curve Diffie-Hellman. The pair can be generated only
         * if the key pair of the client has not been already generated and
         * the public key of the server has not been already set.
         * @throws CryptoException  if the public key of the server has already been set,
         *                          or the client key pair has already been generated,
         *                          or an error occurred while generating the pair.
         */
        void generateServerKeys();

        /**
         * Initializes the cipher used to encrypt, decrypt and authenticate the communications.
         * The key and initialization vector of the cipher are derived from the SHA256 hash
         * of an entropy source. The latter is obtained concatenating:
         * 1) the Elliptic-curve Diffie-Hellman shared secret;
         * 2) the client nonce;
         * 3) the server nonce.
         * At the end of the method, the ECDH key pair that was previously generated
         * is securely destroyed and made unrecoverable.
         * @throws CryptoException         if at least one of the above quantities has not been set/generated,
         *                                 or an error occurred while deriving the secret quantities.
         * @throws SerializationException  if the public key that was set is not represented
         *                                 in a correct binary format. This check does not involve
         *                                 the generated public key.
         */
        void initCipher();

        /**
         * Generates the proof of freshness characterizing the communication
         * session. The proof is obtained concatenating:
         * 1) the username;
         * 2) the client nonce;
         * 3) the server nonce;
         * 4) the Elliptic-curve Diffie-Hellman public key of the client;
         * 5) the Elliptic-curve Diffie-Hellman public key of the server;
         * 6) the certificate of the server.
         * @throws CryptoException         if at least one of the above quantities has not been set/generated,
         *                                 or the certificate is not correctly sized,
         *                                 or an error occurred while deriving the secret quantities.
         * @throws SerializationException  if an error occurred while retrieving a generated public key.
         *                                 This check does not involve the public key that was set.
         */
        void generateFreshnessProof(const std::vector<unsigned char> &certificate);

        /**
         * Increments the sequence number by one, checking for a possible overflow.
         * The sequence number is big enough to cover the number of messages
         * exchanged during an average communication session.
         * If the maximum sequence number has been reached, the communication must be terminated.
         * @throws CryptoException  if the maximum sequence number has been reached and
         *                          the communication must be terminated.
         */
        void incrementSequenceNumber();
};

}

#endif //INC_4INAROW_PLAYER_H
