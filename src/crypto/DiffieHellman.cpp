#include <CryptoException.h>
#include <SerializationException.h>
#include <Utils.h>
#include "DiffieHellman.h"

namespace fourinarow {

void DiffieHellman::generateParameters(EVP_PKEY **parameters) {
    EVP_PKEY_CTX *context = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, nullptr);

    if (!context) {
        throw CryptoException(getOpenSslError());
    }

    if (!EVP_PKEY_paramgen_init(context)) {
        EVP_PKEY_CTX_free(context);
        throw CryptoException(getOpenSslError());
    }

    if (!EVP_PKEY_CTX_set_ec_paramgen_curve_nid(context, this->curve)) {
        EVP_PKEY_CTX_free(context);
        throw CryptoException(getOpenSslError());
    }

    if (!EVP_PKEY_paramgen(context, &(*parameters))) {
        EVP_PKEY_CTX_free(context);
        throw CryptoException(getOpenSslError());
    }

    EVP_PKEY_CTX_free(context);
}

void DiffieHellman::generateKeyPair(EVP_PKEY *parameters) {
    EVP_PKEY_CTX *context = EVP_PKEY_CTX_new(parameters, nullptr);

    if (!context) {
        throw CryptoException(getOpenSslError());
    }

    if (!EVP_PKEY_keygen_init(context)) {
        EVP_PKEY_CTX_free(context);
        throw CryptoException(getOpenSslError());
    }

    if (!EVP_PKEY_keygen(context, &(this->privateKey))) {
        EVP_PKEY_CTX_free(context);
        throw CryptoException(getOpenSslError());
    }

    EVP_PKEY_CTX_free(context);
}

DiffieHellman::DiffieHellman() {
    // Initializations are necessary to avoid problems using the OpenSSL API.
    EVP_PKEY *parameters = nullptr;
    privateKey = nullptr;

    generateParameters(&parameters);

    try {
        generateKeyPair(parameters);
        EVP_PKEY_free(parameters);
    } catch (const CryptoException &exception) {
        EVP_PKEY_free(parameters);
        throw;
    }
}

DiffieHellman::~DiffieHellman() {
    if (privateKey != nullptr) {
        EVP_PKEY_free(privateKey);
    }
}

DiffieHellman::DiffieHellman(DiffieHellman &&that) noexcept : curve(that.curve), privateKey(that.privateKey) {
    that.privateKey = nullptr; // Avoid a call to EVP_PKEY_free() when destructing "that".
}

DiffieHellman& DiffieHellman::operator=(DiffieHellman &&that) noexcept {
    if (privateKey != nullptr) {
        EVP_PKEY_free(privateKey);
    }

    privateKey = that.privateKey;
    that.privateKey = nullptr; // Avoid a call to EVP_PKEY_free() when destructing "that".
    return *this;
}

std::vector<unsigned char> DiffieHellman::getSerializedPublicKey() const {
    unsigned char *publicKeyBuffer = nullptr;
    auto outputSize = i2d_PUBKEY(privateKey, &publicKeyBuffer);

    if (outputSize < 0) {
        throw SerializationException(getOpenSslError());
    }

    std::vector<unsigned char> publicKey(outputSize);
    memcpy(publicKey.data(), publicKeyBuffer, outputSize);

    OPENSSL_free(publicKeyBuffer);
    return publicKey;
}

EVP_PKEY* DiffieHellman::deserializePublicKey(const std::vector<unsigned char> &serializedPeerPublicKey) const {
    const unsigned char *buffer = serializedPeerPublicKey.data();
    EVP_PKEY *peerPublicKey = d2i_PUBKEY(nullptr, &buffer, serializedPeerPublicKey.size());

    if (!peerPublicKey) {
        throw CryptoException(getOpenSslError());
    }

    return peerPublicKey;
}

std::vector<unsigned char> DiffieHellman::deriveSharedSecret(const std::vector<unsigned char> &serializedPeerPublicKey) const {
    if (serializedPeerPublicKey.empty()) {
        throw CryptoException("The peer's public key is empty");
    }

    // Deserialize the peer public key into an EVP_PKEY.
    EVP_PKEY *peerPublicKey = deserializePublicKey(serializedPeerPublicKey);

    // Derive the shared secret.
    EVP_PKEY_CTX *context = EVP_PKEY_CTX_new(privateKey, nullptr);

    if (!context) {
        EVP_PKEY_free(peerPublicKey);
        throw CryptoException(getOpenSslError());
    }

    if (1 != EVP_PKEY_derive_init(context)) {
        EVP_PKEY_free(peerPublicKey);
        EVP_PKEY_CTX_free(context);
        throw CryptoException(getOpenSslError());
    }

    if (1 != EVP_PKEY_derive_set_peer(context, peerPublicKey)) {
        EVP_PKEY_free(peerPublicKey);
        EVP_PKEY_CTX_free(context);
        throw CryptoException(getOpenSslError());
    }

    size_t sharedSecretLength = 0;
    EVP_PKEY_derive(context, nullptr, &sharedSecretLength);
    std::vector<unsigned char> sharedSecret(sharedSecretLength);

    if (1 != EVP_PKEY_derive(context, sharedSecret.data(), &sharedSecretLength)) {
        EVP_PKEY_free(peerPublicKey);
        EVP_PKEY_CTX_free(context);
        throw CryptoException(getOpenSslError());
    }

    EVP_PKEY_CTX_free(context);
    EVP_PKEY_free(peerPublicKey);
    return sharedSecret;
}

}
