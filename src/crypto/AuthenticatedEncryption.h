#ifndef INC_4INAROW_AUTHENTICATEDENCRYPTION_H
#define INC_4INAROW_AUTHENTICATEDENCRYPTION_H

#include <vector>
#include <openssl/evp.h>

namespace fourinarow {

/**
 * Class used to perform authenticated encryption using AES-128 in Galois Counter Mode (GCM).
 * The private key and the initialization vector are held in memory for the entire
 * lifetime of an object, and securely destroyed when the destructor is called.
 */
class AuthenticatedEncryption {
    private:
        const EVP_CIPHER *cipher = EVP_aes_128_gcm();
        std::vector<unsigned char> key;
        std::vector<unsigned char> iv;
    public:
        /**
         * Creates an object able to encrypt and decrypt messages using AES-128 GCM.
         * The key must be on 16 bytes, while the initialization vector must be on 12 bytes.
         * Note that the method makes a copy of the quantities, so it is responsibility
         * of the caller to securely destroy the original ones.
         * @param key  the key.
         * @param iv   the initialization vector.
         * @throws CryptoException  if the key and the initialization vector are wrongly sized.
         */
        AuthenticatedEncryption(std::vector<unsigned char> key, std::vector<unsigned char> iv);

        /**
         * Destroys the object and securely wipes the private key and
         * the initialization vector from memory.
         */
        ~AuthenticatedEncryption();

        /**
         * Move constructs an authenticated encryption object, automatically transferring
         * the ownership of the private key and initialization vector, so that
         * the moved object cannot access them anymore.
         * Calling <code>encrypt()</code> or <code>decrypt()</code> on the moved object
         * results in undefined behaviour.
         * @param that  the authenticated encryption object to move.
         */
        AuthenticatedEncryption(AuthenticatedEncryption &&that) noexcept;

        /**
         * Move assigns an authenticated encryption object, automatically transferring
         * the ownership of the private key and initialization vector, so that
         * the moved object cannot access them anymore.
         * Calling <code>encrypt()</code> or <code>decrypt()</code> on the moved object
         * results in undefined behaviour.
         * @param that  the authenticated encryption object to move.
         */
        AuthenticatedEncryption& operator=(AuthenticatedEncryption &&that) noexcept;

        AuthenticatedEncryption(const AuthenticatedEncryption&) = delete;
        AuthenticatedEncryption& operator=(const AuthenticatedEncryption&) = delete;

        /**
         * Encrypts a plaintext, generating a tag that depends on the given optional
         * additional authenticated data. The ciphertext length is equal to
         * the plaintext length, while the tag is on 128 bits.
         * @param plaintext  the plaintext to encrypt.
         * @param aad        the additional authenticated data (optional).
         * @return           an array of bytes containing the ciphertext concatenated with the tag.
         * @throws CryptoException  if the plaintext is empty, or an error occurs while encrypting
         *                          and generating the tag.
         */
        std::vector<unsigned char> encrypt(const std::vector<unsigned char> &plaintext,
                                           const std::vector<unsigned char> &aad = std::vector<unsigned char>()) const;

        /**
         * Decrypts a ciphertext verifying that the associated tag is valid.
         * @param ciphertextAndTag  an array of bytes containing the ciphertext concatenated with the tag.
         * @param aad               the additional authenticated data (optional).
         * @return                  the decrypted plaintext.
         * @throws CryptoException  if the given array is empty or malformed,
         *                          or an error occurs while decrypting and generating the tag,
         *                          or the tag is not valid.
         */
        std::vector<unsigned char> decrypt(const std::vector<unsigned char> &ciphertextAndTag,
                                           const std::vector<unsigned char> &aad = std::vector<unsigned char>()) const;
};

}

#endif //INC_4INAROW_AUTHENTICATEDENCRYPTION_H
