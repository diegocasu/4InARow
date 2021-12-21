#ifndef INC_4INAROW_CSPRNG_H
#define INC_4INAROW_CSPRNG_H

#include <vector>

namespace fourinarow {

/**
 * Class representing a cryptographically-secure pseudorandom number generator.
 */
class CSPRNG {
    public:
        CSPRNG() = delete;
        ~CSPRNG() = delete;
        CSPRNG(const CSPRNG&) = delete;
        CSPRNG(CSPRNG&&) = delete;
        CSPRNG& operator=(const CSPRNG&) = delete;
        CSPRNG& operator=(CSPRNG&&) = delete;

        /**
         * Generates a given number of random bytes and saves them inside the destination vector.
         * The bytes are generated exploiting <code>RAND_bytes()</code> offered by OpenSSL.
         * @param destination    the destination vector.
         * @param numberOfBytes  the number of bytes to generate.
         * @throws CryptoException  if the destination vector is not big enough to hold the bytes,
         *                          or an error occurs while generating them.
         */
        static void nextBytes(std::vector<unsigned char> &destination, unsigned int numberOfBytes);

        /**
         * Generates a random boolean.
         * @return  a random boolean.
         * @throws CryptoException  if an error occurs while generating the boolean.
         */
        static bool nextBool();
};

}

#endif //INC_4INAROW_CSPRNG_H
