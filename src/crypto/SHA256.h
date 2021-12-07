#ifndef INC_4INAROW_SHA256_H
#define INC_4INAROW_SHA256_H

#include <vector>
#include <openssl/pem.h>

namespace fourinarow {

/**
 * Class representing the SHA-256 hash function.
 */
class SHA256 {
    private:
        static const EVP_MD *hashFunction;
        static const int digestSize;
    public:
        SHA256() = delete;
        ~SHA256() = delete;
        SHA256(const SHA256&) = delete;
        SHA256(SHA256&&) = delete;
        SHA256& operator=(const SHA256&) = delete;
        SHA256& operator=(SHA256&&) = delete;

        /**
         * Hashes the given binary input, producing a digest on 256 bits.
         * @param input  the binary data to hash.
         * @return       the digest on 256 bits.
         * @throws CryptoException  if the input is empty, or an error occurs while hashing.
         */
        static std::vector<unsigned char> hash(const std::vector<unsigned char> &input);
};

}

#endif //INC_4INAROW_SHA256_H
