#include <Utils.h>
#include <CryptoException.h>
#include <Constants.h>
#include "CSPRNG.h"
#include "AuthenticatedEncryption.h"

namespace fourinarow {

AuthenticatedEncryption::AuthenticatedEncryption(std::vector<unsigned char> key) : key(std::move(key)) {
    checkKeySize<CryptoException>(this->key);
}

AuthenticatedEncryption::~AuthenticatedEncryption() {
    if (!key.empty()) {
        cleanse(key);
    }
}

AuthenticatedEncryption::AuthenticatedEncryption(AuthenticatedEncryption &&that) noexcept
: cipher(that.cipher), key(std::move(that.key)) {
    that.cipher = nullptr;
    that.key = std::vector<unsigned char>();
}

AuthenticatedEncryption& AuthenticatedEncryption::operator=(AuthenticatedEncryption &&that) noexcept {
    if (!key.empty()) {
        cleanse(key);
    }

    cipher = that.cipher;
    key = std::move(that.key);
    that.cipher = nullptr;
    that.key = std::vector<unsigned char>();

    return *this;
}

std::vector<unsigned char> AuthenticatedEncryption::encrypt(const std::vector<unsigned char> &plaintext,
                                                            const std::vector<unsigned char> &aad) const {
    if (plaintext.empty()) {
        throw CryptoException("Empty plaintext");
    }

    std::vector<unsigned char> iv(IV_SIZE);
    std::vector<unsigned char> result(IV_SIZE + plaintext.size() + TAG_SIZE);

    CSPRNG::nextBytes(iv, IV_SIZE);
    memcpy(result.data(), iv.data(), IV_SIZE);

    EVP_CIPHER_CTX *context = EVP_CIPHER_CTX_new();
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
    if (1 != EVP_EncryptUpdate(context, result.data() + IV_SIZE, &encryptOutputLength,
                               plaintext.data(), plaintext.size())) {
        EVP_CIPHER_CTX_free(context);
        throw CryptoException(getOpenSslError());
    }
    ciphertextLength = encryptOutputLength;

    if (1 != EVP_EncryptFinal(context, result.data() + IV_SIZE + ciphertextLength, &encryptOutputLength)) {
        EVP_CIPHER_CTX_free(context);
        throw CryptoException(getOpenSslError());
    }
    ciphertextLength += encryptOutputLength;

    // Retrieve the tag.
    if (1 != EVP_CIPHER_CTX_ctrl(context, EVP_CTRL_AEAD_GET_TAG, TAG_SIZE,
                                 result.data() + IV_SIZE + ciphertextLength)) {
        EVP_CIPHER_CTX_free(context);
        throw CryptoException(getOpenSslError());
    }

    EVP_CIPHER_CTX_free(context);
    return result;
}

std::vector<unsigned char> AuthenticatedEncryption::decrypt(const std::vector<unsigned char> &ciphertext,
                                                            const std::vector<unsigned char> &aad) const {
    if (ciphertext.empty()) {
        throw CryptoException("Empty ciphertext");
    }

    if (ciphertext.size() <= IV_SIZE + TAG_SIZE) {
        throw CryptoException("Malformed ciphertext");
    }

    std::vector<unsigned char> iv(IV_SIZE);
    memcpy(iv.data(), ciphertext.data(), IV_SIZE);

    size_t ciphertextLength = ciphertext.size() - IV_SIZE - TAG_SIZE;
    std::vector<unsigned char> plaintext(ciphertextLength);

    EVP_CIPHER_CTX *context = EVP_CIPHER_CTX_new();
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
    if (1 != EVP_DecryptUpdate(context, plaintext.data(), &decryptOutputLength,
                               ciphertext.data() + IV_SIZE, ciphertextLength)) {
        EVP_CIPHER_CTX_free(context);
        throw CryptoException(getOpenSslError());
    }

    // Provide the expected tag.
    if (1 != EVP_CIPHER_CTX_ctrl(context, EVP_CTRL_AEAD_SET_TAG, TAG_SIZE,
                                 (unsigned char*) ciphertext.data() + IV_SIZE + ciphertextLength)) {
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
