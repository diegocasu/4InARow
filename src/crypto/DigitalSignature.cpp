#include <CryptoException.h>
#include <SerializationException.h>
#include <Utils.h>
#include "DigitalSignature.h"

namespace fourinarow {

const EVP_MD* DigitalSignature::hashFunction = EVP_sha256();

DigitalSignature::DigitalSignature(const std::string &path) {
    loadPrivateKey(path);
}

DigitalSignature::~DigitalSignature() {
    if (privateKey != nullptr) {
        EVP_PKEY_free(privateKey);
    }
}

DigitalSignature::DigitalSignature(DigitalSignature &&that) noexcept : privateKey(that.privateKey) {
    that.privateKey = nullptr; // Avoid a call to EVP_PKEY_free() when destructing "that".
}

DigitalSignature& DigitalSignature::operator=(DigitalSignature &&that) noexcept {
    if (privateKey != nullptr) {
        EVP_PKEY_free(privateKey);
    }

    privateKey = that.privateKey;
    that.privateKey = nullptr; // Avoid a call to EVP_PKEY_free() when destructing "that".
    return *this;
}

void DigitalSignature::loadPrivateKey(const std::string &path) {
    FILE *file = fopen(path.data(), "r");

    if (!file) {
        throw CryptoException("Impossible to open the private key file");
    }

    privateKey = PEM_read_PrivateKey(file, nullptr, nullptr, nullptr);
    fclose(file);

    if (!privateKey) {
        throw CryptoException(getOpenSslError());
    }
}

EVP_PKEY* DigitalSignature::loadPublicKey(const std::string &path) {
    FILE *file = fopen(path.data(), "r");

    if (!file) {
        throw CryptoException("Impossible to open the public key file");
    }

    EVP_PKEY *publicKey = PEM_read_PUBKEY(file, nullptr, nullptr, nullptr);
    fclose(file);

    if (!publicKey) {
        throw CryptoException(getOpenSslError());
    }

    return publicKey;
}

std::vector<unsigned char> DigitalSignature::sign(const std::vector<unsigned char> &message) const {
    if (message.empty()) {
        throw CryptoException("Empty message");
    }

    EVP_MD_CTX *context = EVP_MD_CTX_new();

    if (!context) {
        throw CryptoException(getOpenSslError());
    }

    if (1 != EVP_SignInit(context, hashFunction)) {
        EVP_MD_CTX_free(context);
        throw CryptoException(getOpenSslError());
    }

    if (1 != EVP_SignUpdate(context, message.data(), message.size())) {
        EVP_MD_CTX_free(context);
        throw CryptoException(getOpenSslError());
    }

    std::vector<unsigned char> signature(EVP_PKEY_size(privateKey));
    auto length = 0u;

    if (1 != EVP_SignFinal(context, signature.data(), &length, privateKey)) {
        EVP_MD_CTX_free(context);
        throw CryptoException(getOpenSslError());
    }

    EVP_MD_CTX_free(context);
    signature.resize(length);
    return signature;
}

bool DigitalSignature::verify(const std::vector<unsigned char> &message,
                              const std::vector<unsigned char> &signature,
                              const EVP_PKEY *publicKey) {
    if (message.empty()) {
        throw CryptoException("Empty message");
    }

    if (signature.empty()) {
        throw CryptoException("Empty signature");
    }

    if (publicKey == nullptr) {
        throw CryptoException("Empty public key");
    }

    EVP_MD_CTX *context = EVP_MD_CTX_new();

    if (!context) {
        throw CryptoException(getOpenSslError());
    }

    if (1 != EVP_VerifyInit(context, hashFunction)) {
        EVP_MD_CTX_free(context);
        throw CryptoException(getOpenSslError());
    }

    if (1 != EVP_VerifyUpdate(context, message.data(), message.size())) {
        EVP_MD_CTX_free(context);
        throw CryptoException(getOpenSslError());
    }

    auto result = EVP_VerifyFinal(context, signature.data(), signature.size(), (EVP_PKEY*) publicKey);
    EVP_MD_CTX_free(context);

    if (result == -1) {
        throw CryptoException(getOpenSslError());
    }

    if (result == 0) {
        return false;
    }
    return true;
}

bool DigitalSignature::verify(const std::vector<unsigned char> &message,
                              const std::vector<unsigned char> &signature,
                              const std::string &path) {
    EVP_PKEY *publicKey = loadPublicKey(path);

    try {
        auto result = verify(message, signature, publicKey);
        EVP_PKEY_free(publicKey);
        return result;
    } catch (const CryptoException &exception) {
        EVP_PKEY_free(publicKey);
        throw;
    }
}

bool DigitalSignature::verify(const std::vector<unsigned char> &message,
                              const std::vector<unsigned char> &signature,
                              const std::vector<unsigned char> &serializedPublicKey) {
    EVP_PKEY *publicKey = deserializePublicKey(serializedPublicKey);

    try {
        auto result = verify(message, signature, publicKey);
        EVP_PKEY_free(publicKey);
        return result;
    } catch (const CryptoException &exception) {
        EVP_PKEY_free(publicKey);
        throw;
    }
}

std::vector<unsigned char> DigitalSignature::serializePublicKey(const std::string &path) {
    EVP_PKEY *publicKey = loadPublicKey(path);
    unsigned char *buffer = nullptr;
    auto outputSize = i2d_PUBKEY(publicKey, &buffer);

    if (outputSize < 0) {
        EVP_PKEY_free(publicKey);
        throw SerializationException(getOpenSslError());
    }

    std::vector<unsigned char> serializedPublicKey(outputSize);
    memcpy(serializedPublicKey.data(), buffer, outputSize);

    EVP_PKEY_free(publicKey);
    OPENSSL_free(buffer);
    return serializedPublicKey;
}

EVP_PKEY *DigitalSignature::deserializePublicKey(const std::vector<unsigned char> &serializedPublicKey) {
    const unsigned char *buffer = serializedPublicKey.data();
    EVP_PKEY *publicKey = d2i_PUBKEY(nullptr, &buffer, serializedPublicKey.size());

    if (!publicKey) {
        throw CryptoException(getOpenSslError());
    }

    return publicKey;
}

}
