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

        /**
         * Loads a public key in PEM format from a file. It is responsibility
         * of the caller to free the memory allocated to hold the key.
         * @param path  the file path.
         * @throws CryptoException  if the file cannot be opened or the key cannot be loaded.
         */
        static EVP_PKEY* loadPublicKey(const std::string &path);

        /**
         * Parses a public key in binary format, returning a representation
         * usable by the OpenSSL API. It is responsibility of the caller
         * to free the memory allocated to hold the key.
         * @param publicKey  the public key, in binary format.
         * @return           the public key, in OpenSSL format.
         * @throws CryptoException         if an error occurred while initializing the OpenSSL API.
         * @throws SerializationException  if the public key is not represented in a correct binary format.
         */
        static EVP_PKEY* deserializePublicKey(const std::vector<unsigned char> &serializedPublicKey);
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

        /**
         * Move constructs a digital signature, automatically transferring the ownership of
         * the private key, so that the moved object cannot access it anymore.
         * Calling <code>sign()</code> on the moved object results in undefined behaviour.
         * @param that  the digital signature to move.
         */
        DigitalSignature(DigitalSignature &&that) noexcept;

        /**
         * Move assigns a digital signature, automatically transferring the ownership of
         * the private key, so that the moved object cannot access it anymore.
         * Calling <code>sign()</code> on the moved object results in undefined behaviour.
         * @param that  the digital signature to move.
         */
        DigitalSignature& operator=(DigitalSignature &&that) noexcept;

        DigitalSignature(const DigitalSignature&) = delete;
        DigitalSignature& operator=(const DigitalSignature&) = delete;

        /**
         * Signs the SHA256 digest of a message.
         * @param message  the message to sign.
         * @return         the digital signature of the message.
         * @throws CryptoException  if the message is empty, or an error occurred
         *                          while creating the signature.
         */
        std::vector<unsigned char> sign(const std::vector<unsigned char> &message) const;

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

        /**
         * Verifies a digital signature of a message using a public key stored
         * in PEM format in a file. The verification is done using
         * the SHA256 digest of the message.
         * @param message    the message.
         * @param signature  the digital signature to verify.
         * @param path       the path of the file containing the public key.
         * @return           true if the signature is valid, false otherwise.
         * @throws CryptoException  if the message or the signature are empty;
         *                          if the file cannot be opened or the public key cannot be loaded;
         *                          if an error occurred while verifying the signature.
         */
        static bool verify(const std::vector<unsigned char> &message,
                           const std::vector<unsigned char> &signature,
                           const std::string &path);

        /**
         * Verifies a digital signature of a message using a serialized public key.
         * The verification is done using the SHA256 digest of the message.
         * @param message    the message.
         * @param signature  the digital signature to verify.
         * @param publicKey  the public key used to verify the signature.
         * @return           true if the signature is valid, false otherwise.
         * @throws CryptoException        if the message, the signature or the public key are empty;
         *                                if an error occurred while verifying the signature.
         * @throws SerializationException if the public key is not represented in a correct binary format.
         */
        static bool verify(const std::vector<unsigned char> &message,
                           const std::vector<unsigned char> &signature,
                           const std::vector<unsigned char> &serializedPublicKey);

        /**
         * Loads and serializes a public key stored in PEM format in a file.
         * @param path  the path of the file containing the public key.
         * @return      the serialized key.
         * @throws CryptoException         if the file cannot be opened or the key cannot be loaded.
         * @throws SerializationException  if an error occurred while serializing the key.
         */
        static std::vector<unsigned char> serializePublicKey(const std::string &path);
};

}

#endif //INC_4INAROW_DIGITALSIGNATURE_H
