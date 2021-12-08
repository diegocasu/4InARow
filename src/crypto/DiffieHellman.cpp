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
        EVP_PKEY_free(parameters);
        throw CryptoException(getOpenSslError());
    }

    if (!EVP_PKEY_keygen_init(context)) {
        EVP_PKEY_free(parameters);
        EVP_PKEY_CTX_free(context);
        throw CryptoException(getOpenSslError());
    }

    if (!EVP_PKEY_keygen(context, &(this->privateKey))) {
        EVP_PKEY_free(parameters);
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
    generateKeyPair(parameters);
    EVP_PKEY_free(parameters);
}

DiffieHellman::~DiffieHellman() {
    EVP_PKEY_free(privateKey);
}

std::vector<unsigned char> DiffieHellman::getSerializedPublicKey() const {
    int length = i2d_PublicKey(privateKey, 0);
    if (length < 0) {
        throw SerializationException(getOpenSslError());
    }

    std::vector<unsigned char> publicKey(length);
    unsigned char *publicKeyPointer = publicKey.data();

    length = i2d_PublicKey(privateKey, &publicKeyPointer);
    if (length < 0) {
        throw SerializationException(getOpenSslError());
    }

    return publicKey;
}

EVP_PKEY* DiffieHellman::deserializePublicKey(const std::vector<unsigned char> &serializedPeerPublicKey) const {
    EC_KEY *ecKey = EC_KEY_new_by_curve_name(curve);
    if (!ecKey) {
        throw CryptoException(getOpenSslError());
    }

    EVP_PKEY *publicKey = EVP_PKEY_new();
    if (!publicKey) {
        EC_KEY_free(ecKey);
        throw CryptoException(getOpenSslError());
    }

    if (!EVP_PKEY_assign_EC_KEY(publicKey, ecKey)) {
        EC_KEY_free(ecKey);
        EVP_PKEY_free(publicKey);
        throw CryptoException(getOpenSslError());
    }

    unsigned char const *binaryPublicKeyPointer = serializedPeerPublicKey.data();
    EVP_PKEY *success = d2i_PublicKey(EVP_PKEY_EC, &publicKey, &binaryPublicKeyPointer, serializedPeerPublicKey.size());
    if (!success) {
        EC_KEY_free(ecKey);
        EVP_PKEY_free(publicKey);
        throw SerializationException(getOpenSslError());
    }

    EC_KEY_free(ecKey);
    return publicKey;
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
