#include <climits>
#include <string.h>
#include <Constants.h>
#include <Utils.h>
#include <CryptoException.h>
#include <SerializationException.h>
#include <CSPRNG.h>
#include <SHA256.h>
#include "Player.h"

namespace fourinarow {

fourinarow::Player::Player()
    : status(Status::OFFLINE),
      clientKeys(nullptr),
      serverKeys(nullptr),
      cipher(nullptr),
      sequenceNumberReads(0),
      sequenceNumberWrites(0),
      matchmakingInitiator(false) {}

const std::string& Player::getUsername() const {
    return username;
}

Player::Status Player::getStatus() const {
    return status;
}

const std::vector<unsigned char>& Player::getClientNonce() const {
    return clientNonce;
}

const std::vector<unsigned char>& Player::getServerNonce() const {
    return serverNonce;
}

const std::string& Player::getMatchmakingPlayer() const {
    return matchmakingPlayer;
}

const std::vector<unsigned char>& Player::getClientFreshnessProof() const {
    return clientFreshnessProof;
}

const std::vector<unsigned char>& Player::getServerFreshnessProof() const {
    return serverFreshnessProof;
}

std::vector<unsigned char> Player::getClientPublicKey() const {
    if (clientKeys != nullptr) {
        return clientKeys->getSerializedPublicKey();
    }
    return clientPublicKey;
}

const std::vector<unsigned char> Player::getServerPublicKey() const {
    if (serverKeys != nullptr) {
        return serverKeys->getSerializedPublicKey();
    }
    return serverPublicKey;
}

const AuthenticatedEncryption& Player::getCipher() const {
    if (cipher == nullptr) {
        throw CryptoException("The cipher has not been generated yet");
    }
    return *cipher;
}

uint32_t Player::getSequenceNumberReads() const {
    return sequenceNumberReads;
}

uint32_t Player::getSequenceNumberWrites() const {
    return sequenceNumberWrites;
}

bool Player::isMatchmakingInitiator() const {
    return matchmakingInitiator;
}

void Player::setStatus(Player::Status newStatus) {
    status = newStatus;
}

void Player::setMatchmakingPlayer(std::string newMatchmakingPlayer) {
    matchmakingPlayer = std::move(newMatchmakingPlayer);
}

void Player::setAsMatchmakingInitiator(bool initiator) {
    matchmakingInitiator = initiator;
}

void Player::setUsername(std::string newUsername) {
    checkUsernameValidity<SerializationException>(newUsername);
    username = std::move(newUsername);
}

void Player::setClientNonce(std::vector<unsigned char> nonce) {
    checkNonceSize<SerializationException>(nonce);
    clientNonce = std::move(nonce);
}

void Player::setServerNonce(std::vector<unsigned char> nonce) {
    checkNonceSize<SerializationException>(nonce);
    serverNonce = std::move(nonce);
}

void Player::setClientPublicKey(std::vector<unsigned char> publicKey) {
    checkEcdhPublicKeySize<SerializationException>(publicKey);

    if (clientKeys != nullptr) {
        throw CryptoException("The key pair of the client has already been generated, "
                              "so the public key cannot be set");
    }

    if (!serverPublicKey.empty()) {
        throw CryptoException("The public key of the server has already been set. "
                              "The keys of the client must be generated, not set");
    }

    clientPublicKey = std::move(publicKey);
}

void Player::setServerPublicKey(std::vector<unsigned char> publicKey) {
    checkEcdhPublicKeySize<SerializationException>(publicKey);

    if (serverKeys != nullptr) {
        throw CryptoException("The key pair of the server has already been generated, "
                              "so the public key cannot be set");
    }

    if (!clientPublicKey.empty()) {
        throw CryptoException("The public key of the client has already been set. "
                              "The keys of the server must be generated, not set");
    }

    serverPublicKey = std::move(publicKey);
}

void Player::generateClientNonce() {
    clientNonce.resize(NONCE_SIZE);
    CSPRNG::nextBytes(clientNonce, NONCE_SIZE);
}

void Player::generateServerNonce() {
    serverNonce.resize(NONCE_SIZE);
    CSPRNG::nextBytes(serverNonce, NONCE_SIZE);
}

void Player::generateClientKeys() {
    if (!clientPublicKey.empty()) {
        throw CryptoException("The public key of the client has already been set, so it cannot be generated");
    }

    if (serverKeys != nullptr) {
        throw CryptoException("The key pair of the server has already been generated. "
                              "The public key of the client must be set, not generated");
    }

    clientKeys = std::make_unique<DiffieHellman>();
}

void Player::generateServerKeys() {
    if (!serverPublicKey.empty()) {
        throw CryptoException("The public key of the server has already been set, so it cannot be generated");
    }

    if (clientKeys != nullptr) {
        throw CryptoException("The key pair of the client has already been generated. "
                              "The public key of the server must be set, not generated");
    }

    serverKeys = std::make_unique<DiffieHellman>();
}

void Player::checkIfClientNonceInitialized() const {
    if (clientNonce.empty()) {
        throw CryptoException("The client nonce has not been set or generated yet");
    }
}

void Player::checkIfServerNonceInitialized() const {
    if (serverNonce.empty()) {
        throw CryptoException("The server nonce has not been set or generated yet");
    }
}

void Player::checkIfClientKeyInitialized() const {
    if (clientKeys == nullptr && clientPublicKey.empty()) {
        throw CryptoException("The key pair/public key of the client has not been generated/set yet, "
                              "or the key pair has been destroyed");
    }
}

void Player::checkIfServerKeyInitialized() const {
    if (serverKeys == nullptr && serverPublicKey.empty()) {
        throw CryptoException("The key pair/public key of the server has not been generated/set yet, "
                              "or the key pair has been destroyed");
    }
}

void Player::initCipher() {
    checkIfClientNonceInitialized();
    checkIfServerNonceInitialized();
    checkIfClientKeyInitialized();
    checkIfServerKeyInitialized();

    std::vector<unsigned char> sharedSecret;

    if (clientKeys != nullptr) {
        sharedSecret = clientKeys->deriveSharedSecret(serverPublicKey);
    } else { // serverKeys != nullptr
        sharedSecret = serverKeys->deriveSharedSecret(clientPublicKey);
    }

    // Concatenate the shared secret, the client nonce and the server nonce to generate the entropy source.
    std::vector<unsigned char> entropySource;
    entropySource.reserve(sharedSecret.size() + clientNonce.size() + serverNonce.size());
    concatenate(entropySource, sharedSecret, clientNonce, serverNonce);

    // Derive the key for the cipher.
    auto secretBlock = SHA256::hash(entropySource);

    /*
     * Security check in case the symmetric cipher is changed carelessly.
     * It never throws if KEY_SIZE is compliant with AES-128 GCM.
     */
    if (secretBlock.size() < KEY_SIZE) {
        throw CryptoException("The secret block is too small to extract the key");
    }

    cipher = std::make_unique<AuthenticatedEncryption>(std::vector<unsigned char>(secretBlock.begin(),
                                                                                  secretBlock.begin() + KEY_SIZE));

    // Cleansing.
    if (clientKeys != nullptr) {
        clientKeys.reset();
        clientKeys = nullptr;
    } else { // serverKeys != nullptr
        serverKeys.reset();
        serverKeys = nullptr;
    }
    cleanse(sharedSecret);
    cleanse(entropySource);
    cleanse(secretBlock);
}

void Player::generateServerFreshnessProof() {
    checkIfClientNonceInitialized();
    checkIfServerKeyInitialized();

    serverFreshnessProof.reserve(NONCE_SIZE + ECDH_PUBLIC_KEY_SIZE);
    concatenate(serverFreshnessProof, clientNonce, getServerPublicKey());
}

void Player::generateClientFreshnessProof() {
    checkIfServerNonceInitialized();
    checkIfClientKeyInitialized();

    clientFreshnessProof.reserve(NONCE_SIZE + ECDH_PUBLIC_KEY_SIZE);
    concatenate(clientFreshnessProof, serverNonce, getClientPublicKey());
}

void Player::incrementSequenceNumberReads() {
    if (sequenceNumberReads == UINT_MAX) {
        throw CryptoException("Max sequence number for reading messages reached");
    }
    sequenceNumberReads++;
}

void Player::incrementSequenceNumberWrites() {
    if (sequenceNumberWrites == UINT_MAX) {
        throw CryptoException("Max sequence number for writing messages reached");
    }
    sequenceNumberWrites++;
}

}
