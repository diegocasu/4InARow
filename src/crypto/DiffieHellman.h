#ifndef INC_4INAROW_DIFFIEHELLMAN_H
#define INC_4INAROW_DIFFIEHELLMAN_H

#include <openssl/evp.h>
#include <openssl/pem.h>
#include <vector>
#include <ostream>

namespace fourinarow {

/**
 * Class implementing the operations needed to perform a key exchange
 * with Elliptic Curve Diffie-Hellman. It allows to generate private-public
 * key pairs using the standardized prime256v1 curve and to generate a shared secret
 * using the public key of another party. The private key is held in memory for the
 * entire lifetime of an object, and securely destroyed when the destructor is called.
 */
class DiffieHellman {
    private:
        const int curve = NID_X9_62_prime256v1;
        EVP_PKEY *privateKey;

        /**
         * Generates the parameters needed to create the private-public
         * key pair using the prime256v1 curve.
         * @param parameters  the pointer to the pointer that will hold the parameters.
         * @throws  CryptoException  if an error occurred while generating the parameters.
         */
        void generateParameters(EVP_PKEY **parameters);

        /**
         * Generates the private-public key pair starting from the given parameters.
         * @param parameters  the parameters used to generate the key pair.
         * @throws  CryptoException  if an error occurred while generating the pair.
         */
        void generateKeyPair(EVP_PKEY *parameters);

        /**
         * Parses a public key in binary format, returning a representation
         * usable by the OpenSSL API.
         * @param serializedPeerPublicKey  the public key, in binary format.
         * @return                         the public key, in OpenSSL format.
         * @throws  CryptoException  if an error occurred while converting the public key.
         */
        EVP_PKEY* parsePublicKey(const std::vector<unsigned char> &serializedPeerPublicKey) const;
    public:
        /**
         * Creates a private-public key pair using Elliptic Curve Diffie-Hellman
         * and the standardized prime256v1 curve.
         * @throws  CryptoException  if an error occurred while generating the pair.
         */
        DiffieHellman();

        /**
         * Destroys the object and securely wipes the private key from memory.
         */
        ~DiffieHellman();

        DiffieHellman(const DiffieHellman&) = delete;
        DiffieHellman(DiffieHellman&&) = delete;
        DiffieHellman& operator=(const DiffieHellman&) = delete;
        DiffieHellman& operator=(DiffieHellman&&) = delete;

        /**
         * Returns the public key in binary format, ready to be
         * sent through a socket.
         * @return  the public key, in binary format.
         * @throws  CryptoException  if an error occurred while serializing the public key.
         */
        std::vector<unsigned char> getSerializedPublicKey() const;

        /**
         * Derives a shared secret using the private key held by the object
         * and the given public key.
         * @param serializedPeerPublicKey  the public key of the peer, in binary format.
         * @return                         the shared secret, in binary format.
         * @throws  CryptoException  if the given public key is empty, or an error occurred
         *                           while deriving the shared secret.
         */
        std::vector<unsigned char> deriveSharedSecret(const std::vector<unsigned char> &serializedPeerPublicKey) const;
};

}

#endif //INC_4INAROW_DIFFIEHELLMAN_H
