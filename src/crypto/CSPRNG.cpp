#include <openssl/rand.h>
#include "CryptoException.h"
#include "Utils.h"
#include "CSPRNG.h"

namespace fourinarow {

void fourinarow::CSPRNG::nextBytes(std::vector<unsigned char> &destination, unsigned int numberOfBytes) {
    if (destination.size() < numberOfBytes) {
        throw CryptoException("The destination vector is too small");
    }

    /*
     * OpenSSL manages automatically the (re-)seeding of the RNG
     * (at least in OpenSSL 1.1.1+), so there is no need to call RAND_poll().
     */
    if (1 != RAND_bytes(destination.data(), numberOfBytes)) {
        throw CryptoException(getOpenSslError());
    }
}

bool fourinarow::CSPRNG::nextBool() {
    uint8_t randomInt;

    if (1 != RAND_bytes(&randomInt, sizeof(uint8_t))) {
        throw CryptoException(getOpenSslError());
    }

    if (randomInt <= 127) {
        return false;
    }
    return true;
}

}
