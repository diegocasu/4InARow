#include <CryptoException.h>
#include <Utils.h>
#include "DigitalSignature.h"

namespace fourinarow {

const EVP_MD* DigitalSignature::hashFunction = EVP_sha256();

DigitalSignature::DigitalSignature(const std::string &path) {
    loadPrivateKey(path);
}

DigitalSignature::~DigitalSignature() {
    EVP_PKEY_free(privateKey);
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

std::vector<unsigned char> DigitalSignature::sign(const std::vector<unsigned char> &message) {
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

}
