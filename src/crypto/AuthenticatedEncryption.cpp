#include <Utils.h>
#include <CryptoException.h>
#include <Constants.h>
#include "AuthenticatedEncryption.h"

namespace fourinarow {

AuthenticatedEncryption::AuthenticatedEncryption(std::vector<unsigned char> key, std::vector<unsigned char> iv)
: key(std::move(key)), iv(std::move(iv)) {
    if (this->key.empty()) {
        throw CryptoException("Key is empty");
    }

    if (this->iv.empty()) {
        throw CryptoException("IV is empty");
    }

    if (this->key.size() != KEY_SIZE) {
        std::string errorMessage("The key size must be exactly ");
        errorMessage.append(std::to_string(KEY_SIZE));
        errorMessage.append(". Key size: ");
        errorMessage.append(std::to_string(this->key.size()));
        throw CryptoException(errorMessage);
    }

    if (this->iv.size() != IV_SIZE) {
        std::string errorMessage("The IV size must be exactly ");
        errorMessage.append(std::to_string(IV_SIZE));
        errorMessage.append(". IV size: ");
        errorMessage.append(std::to_string(this->iv.size()));
        throw CryptoException(errorMessage);
    }
}

AuthenticatedEncryption::~AuthenticatedEncryption() {
    if (!key.empty()) {
        cleanse(key);
    }
    if (!iv.empty()) {
        cleanse(iv);
    }
}

AuthenticatedEncryption::AuthenticatedEncryption(AuthenticatedEncryption &&that) noexcept
: cipher(that.cipher), key(std::move(that.key)), iv(std::move(that.iv)) {
    that.cipher = nullptr;
    that.key = std::vector<unsigned char>();
    that.iv = std::vector<unsigned char>();
}

std::vector<unsigned char> AuthenticatedEncryption::encrypt(const std::vector<unsigned char> &plaintext,
                                                            const std::vector<unsigned char> &aad) {
    if (plaintext.empty()) {
        throw CryptoException("Empty plaintext");
    }

    EVP_CIPHER_CTX *context = EVP_CIPHER_CTX_new();
    std::vector<unsigned char> result(plaintext.size() + TAG_SIZE);
    auto ciphertextLength = 0;
    auto encryptOutputLength = 0;

    if (!context) {
        throw CryptoException(getOpenSslError());
    }

    if (1 != EVP_EncryptInit(context, cipher, key.data(), iv.data())) {
        EVP_CIPHER_CTX_free(context);
        throw CryptoException(getOpenSslError());
    }

    // Provide optional additional authenticated data.
    if (!aad.empty()) {
        auto dummyLength = 0;
        if (1 != EVP_EncryptUpdate(context, nullptr, &dummyLength, aad.data(), aad.size())) {
            EVP_CIPHER_CTX_free(context);
            throw CryptoException(getOpenSslError());
        }
    }

    // Provide the plaintext.
    if (1 != EVP_EncryptUpdate(context, result.data(), &encryptOutputLength, plaintext.data(), plaintext.size())) {
        EVP_CIPHER_CTX_free(context);
        throw CryptoException(getOpenSslError());
    }
    ciphertextLength = encryptOutputLength;

    if (1 != EVP_EncryptFinal(context, result.data() + ciphertextLength, &encryptOutputLength)) {
        EVP_CIPHER_CTX_free(context);
        throw CryptoException(getOpenSslError());
    }
    ciphertextLength += encryptOutputLength;

    // Retrieve the tag.
    if (1 != EVP_CIPHER_CTX_ctrl(context, EVP_CTRL_AEAD_GET_TAG, TAG_SIZE, result.data() + ciphertextLength)) {
        EVP_CIPHER_CTX_free(context);
        throw CryptoException(getOpenSslError());
    }

    EVP_CIPHER_CTX_free(context);
    return result;
}

std::vector<unsigned char> AuthenticatedEncryption::decrypt(const std::vector<unsigned char> &ciphertextAndTag,
                                                            const std::vector<unsigned char> &aad) {
    if (ciphertextAndTag.empty()) {
        throw CryptoException("Empty ciphertext and tag");
    }

    if (ciphertextAndTag.size() <= TAG_SIZE) {
        throw CryptoException("Malformed ciphertext and tag");
    }

    EVP_CIPHER_CTX *context = EVP_CIPHER_CTX_new();
    size_t ciphertextLength = ciphertextAndTag.size() - TAG_SIZE;
    std::vector<unsigned char> plaintext(ciphertextLength);
    auto decryptOutputLength = 0;

    if (!context) {
        throw CryptoException(getOpenSslError());
    }

    if (1 != EVP_DecryptInit(context, cipher, key.data(), iv.data())) {
        EVP_CIPHER_CTX_free(context);
        throw CryptoException(getOpenSslError());
    }

    // Provide optional additional authenticated data.
    if (!aad.empty()) {
        auto dummyLength = 0;
        if (1 != EVP_DecryptUpdate(context, nullptr, &dummyLength, aad.data(), aad.size())) {
            EVP_CIPHER_CTX_free(context);
            throw CryptoException(getOpenSslError());
        }
    }

    // Provide the ciphertext.
    if (1 != EVP_DecryptUpdate(context, plaintext.data(), &decryptOutputLength, ciphertextAndTag.data(), ciphertextLength)) {
        EVP_CIPHER_CTX_free(context);
        throw CryptoException(getOpenSslError());
    }

    // Provide the expected tag.
    if (1 != EVP_CIPHER_CTX_ctrl(context, EVP_CTRL_AEAD_SET_TAG, TAG_SIZE,
                                 (unsigned char*) ciphertextAndTag.data() + ciphertextLength)) {
        EVP_CIPHER_CTX_free(context);
        throw CryptoException(getOpenSslError());
    }

    if (1 != EVP_DecryptFinal(context, plaintext.data() + decryptOutputLength, &decryptOutputLength)) {
        EVP_CIPHER_CTX_free(context);
        throw CryptoException("Tag mismatch");
    }

    EVP_CIPHER_CTX_free(context);
    return plaintext;
}

}
