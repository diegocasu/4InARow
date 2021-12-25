#ifndef INC_4INAROW_AUTHENTICATEDENCRYPTION_H
#define INC_4INAROW_AUTHENTICATEDENCRYPTION_H

#include <vector>
#include <openssl/evp.h>

namespace fourinarow {

/**
 * Class used to perform authenticated encryption using AES-128 in Galois Counter Mode (GCM).
 * The private key is held in memory for the entire lifetime of an object,
 * and securely destroyed when the destructor is called.
 * The initialization vector is randomly generated at each encryption.
 */
class AuthenticatedEncryption {
    private:
        const EVP_CIPHER *cipher = EVP_aes_128_gcm();
        std::vector<unsigned char> key;
    public:
        /**
         * Creates an object able to encrypt and decrypt messages using AES-128 GCM.
         * The given key must be on 16 bytes.
         * Note that the method makes a copy of the key, so it is responsibility
         * of the caller to securely destroy the original one.
         * @param key  the key.
         * @throws CryptoException  if the key is wrongly sized.
         */
        explicit AuthenticatedEncryption(std::vector<unsigned char> key);

        /**
         * Destroys the object and securely wipes the private key from memory.
         */
        ~AuthenticatedEncryption();

        /**
         * Move constructs an authenticated encryption object, automatically transferring
         * the ownership of the private key, so that the moved object cannot access it anymore.
         * Calling <code>encrypt()</code> or <code>decrypt()</code> on the moved object
         * results in undefined behaviour.
         * @param that  the authenticated encryption object to move.
         */
        AuthenticatedEncryption(AuthenticatedEncryption &&that) noexcept;

        /**
         * Move assigns an authenticated encryption object, automatically transferring
         * the ownership of the private key, so that the moved object cannot access it anymore.
         * Calling <code>encrypt()</code> or <code>decrypt()</code> on the moved object
         * results in undefined behaviour.
         * @param that  the authenticated encryption object to move.
         */
        AuthenticatedEncryption& operator=(AuthenticatedEncryption &&that) noexcept;

        AuthenticatedEncryption(const AuthenticatedEncryption&) = delete;
        AuthenticatedEncryption& operator=(const AuthenticatedEncryption&) = delete;

        /**
         * Encrypts a plaintext using AES-128 GCM.
         * The method extracts a random initialization vector of 12 bytes at each encryption and
         * generates a tag of 128 bits that depends on the given additional authenticated data.
         * The returned result is a concatenation of the IV, the ciphertext and the tag, where
         * the ciphertext length is equal to the plaintext length.
         * @param plaintext  the plaintext to encrypt.
         * @param aad        the additional authenticated data (optional).
         * @return           an array of bytes containing the concatenation of the IV, the ciphertext and the tag.
         * @throws CryptoException  if the plaintext is empty, or an error occurs while encrypting
         *                          and generating the tag.
         */
        std::vector<unsigned char> encrypt(const std::vector<unsigned char> &plaintext,
                                           const std::vector<unsigned char> &aad = std::vector<unsigned char>()) const;

        /**
         * Decrypts a ciphertext verifying that the associated tag is valid.
         * The method expects an input array containing the concatenation of the IV, the ciphertext and the tag.
         * @param ciphertext  an array of bytes containing the concatenation of the IV, the ciphertext and the tag.
         * @param aad         the additional authenticated data (optional).
         * @return            the decrypted plaintext.
         * @throws CryptoException  if the given array is empty or malformed,
         *                          or an error occurs while decrypting and generating the tag,
         *                          or the tag is not valid.
         */
        std::vector<unsigned char> decrypt(const std::vector<unsigned char> &ciphertext,
                                           const std::vector<unsigned char> &aad = std::vector<unsigned char>()) const;
};

}

#endif //INC_4INAROW_AUTHENTICATEDENCRYPTION_H
