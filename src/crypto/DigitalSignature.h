#ifndef INC_4INAROW_DIGITALSIGNATURE_H
#define INC_4INAROW_DIGITALSIGNATURE_H

#include <string>
#include <vector>
#include <openssl/pem.h>

namespace fourinarow {

/**
 * Class used to digitally sign binary data. It allows to create signatures using
 * a private key loaded at construction time from a file, and to verify a signature
 * using a public key passed at run-time. The signing and verification procedures
 * are performed on the SHA256 digest of the given data.
 * The private key is held in memory for the entire lifetime of an object,
 * and securely destroyed when the destructor is called.
 */
class DigitalSignature {
    private:
        static const EVP_MD *hashFunction;
        EVP_PKEY *privateKey;

        /**
         * Loads a private key in PEM format from a file. If the file is password protected,
         * the user will be prompted to insert the password.
         * @param path  the file path.
         * @throws CryptoException  if the file cannot be opened or the key cannot be loaded.
         */
        void loadPrivateKey(const std::string &path);
    public:
        /**
         * Creates a digital signature object that will use the private key saved in the
         * given file. The key must be in PEM format. If the file is password protected,
         * the user will be prompted to insert the password.
         * @param path  the file path.
         * @throws CryptoException  if the file cannot be opened or the key cannot be loaded.
         */
        explicit DigitalSignature(const std::string &path);

        /**
         * Destroys the object and securely wipes the private key from memory.
         */
        ~DigitalSignature();

        DigitalSignature(const DigitalSignature&) = delete;
        DigitalSignature(DigitalSignature&&) = delete;
        DigitalSignature& operator=(const DigitalSignature&) = delete;
        DigitalSignature& operator=(DigitalSignature&&) = delete;

        /**
         * Signs the SHA256 digest of a message.
         * @param message  the message to sign.
         * @return         the digital signature of the message.
         * @throws CryptoException  if the message is empty, or an error occurred
         *                          while creating the signature.
         */
        std::vector<unsigned char> sign(const std::vector<unsigned char> &message);

        /**
         * Verifies a digital signature of a message using a public key.
         * The verification is done using the SHA256 digest of the message.
         * @param message    the message.
         * @param signature  the digital signature to verify.
         * @param publicKey  the public key used to verify the signature.
         * @return           true if the signature is valid, false otherwise.
         * @throws CryptoException  if the message, the signature or the public key are empty;
         *                          if an error occurred while verifying the signature.
         */
        static bool verify(const std::vector<unsigned char> &message,
                           const std::vector<unsigned char> &signature,
                           const EVP_PKEY *publicKey);
};

}

#endif //INC_4INAROW_DIGITALSIGNATURE_H