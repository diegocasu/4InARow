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
      sequenceNumber(0),
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

const std::vector<unsigned char>& Player::getFreshnessProof() const {
    return freshnessProof;
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

uint32_t Player::getSequenceNumber() const {
    return sequenceNumber;
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
        throw CryptoException("The key pair of the client has already been generated, so the public key cannot be set");
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
        throw CryptoException("The key pair of the server has already been generated, so the public key cannot be set");
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

void Player::checkIfCryptoAttributesInitialized() const {
    if (clientNonce.empty()) {
        throw CryptoException("The client nonce has not been set or generated yet");
    }

    if (serverNonce.empty()) {
        throw CryptoException("The server nonce has not been set or generated yet");
    }

    if (clientKeys == nullptr && clientPublicKey.empty()) {
        throw CryptoException("The key pair/public key of the client has not been generated/set yet, "
                              "or the key pair has been destroyed");
    }

    if (serverKeys == nullptr && serverPublicKey.empty()) {
        throw CryptoException("The key pair/public key of the server has not been generated/set yet, "
                              "or the key pair has been destroyed");
    }
}

void Player::initCipher() {
    checkIfCryptoAttributesInitialized();
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

    // Derive the key and the initialization vector for the cipher.
    auto secretBlock = SHA256::hash(entropySource);

    /*
     * Security check in case the symmetric cipher is changed carelessly.
     * It never throws if KEY_SIZE and IV_SIZE are chosen to be compliant with AES-128.
     */
    if (secretBlock.size() < KEY_SIZE + IV_SIZE) {
        throw CryptoException("The secret block is too small to extract both the key and the IV");
    }

    cipher = std::make_unique<AuthenticatedEncryption>(std::vector<unsigned char>(secretBlock.begin(),
                                                                                      secretBlock.begin() + KEY_SIZE),
                                                       std::vector<unsigned char>(secretBlock.begin() + KEY_SIZE,
                                                                                     secretBlock.begin() + KEY_SIZE + IV_SIZE));
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

void Player::generateFreshnessProof(const std::vector<unsigned char> &certificate) {
    if (username.empty()) {
        throw CryptoException("The username has not been set yet");
    }

    checkIfCryptoAttributesInitialized();
    checkCertificateSize<CryptoException>(certificate);

    std::vector<unsigned char> usernameBytes(username.size());
    memcpy(usernameBytes.data(), username.data(), username.size());

    freshnessProof.reserve(username.size() + 2*NONCE_SIZE + 2*ECDH_PUBLIC_KEY_SIZE + certificate.size());
    concatenate(freshnessProof, usernameBytes, clientNonce, serverNonce,
                getClientPublicKey(), getServerPublicKey(), certificate);
}

void Player::generateFreshnessProofP2P() {
    checkIfCryptoAttributesInitialized();
    freshnessProof.reserve(2*NONCE_SIZE + 2*ECDH_PUBLIC_KEY_SIZE);
    concatenate(freshnessProof, clientNonce, serverNonce, getClientPublicKey(), getServerPublicKey());
}

void Player::incrementSequenceNumber() {
    if (sequenceNumber > UINT_MAX - 1) {
        throw CryptoException("Max sequence number reached");
    }
    sequenceNumber++;
}

}
