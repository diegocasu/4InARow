#include <CryptoException.h>
#include <Utils.h>
#include "SHA256.h"

namespace fourinarow {

const EVP_MD* SHA256::hashFunction = EVP_sha256();

const int SHA256::digestSize = EVP_MD_size(hashFunction);

std::vector<unsigned char> SHA256::hash(const std::vector<unsigned char> &input) {
    if (input.empty()) {
        throw CryptoException("Empty data");
    }

    EVP_MD_CTX *context = EVP_MD_CTX_new();
    std::vector<unsigned char> digest(digestSize);

    if (!context) {
        throw CryptoException(getOpenSslError());
    }

    if (1 != EVP_DigestInit(context, hashFunction)) {
        EVP_MD_CTX_free(context);
        throw CryptoException(getOpenSslError());
    }

    if (1 != EVP_DigestUpdate(context, input.data(), input.size())) {
        EVP_MD_CTX_free(context);
        throw CryptoException(getOpenSslError());
    }

    auto dummyLength = 0u;
    if (1 != EVP_DigestFinal(context, digest.data(), &dummyLength)) {
        EVP_MD_CTX_free(context);
        throw CryptoException(getOpenSslError());
    }

    EVP_MD_CTX_free(context);
    return digest;
}

}
