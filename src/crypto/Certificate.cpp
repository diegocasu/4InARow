#include <CryptoException.h>
#include "Certificate.h"
#include <Utils.h>

namespace fourinarow {

Certificate::Certificate(const X509 *rawCertificate) : rawCertificate(rawCertificate) {
    if (rawCertificate == nullptr) {
        throw CryptoException("Null pointer to the certificate");
    }
}

Certificate::~Certificate() {
    if (rawCertificate != nullptr) {
        X509_free((X509*) rawCertificate);
    }
}

Certificate::Certificate(Certificate &&that) noexcept : rawCertificate(that.rawCertificate) {
    that.rawCertificate = nullptr; // Avoid to call X509_free() multiple times.
}

Certificate& Certificate::operator=(Certificate &&that) noexcept {
    if (rawCertificate != nullptr) {
        X509_free((X509*) rawCertificate);
    }

    rawCertificate = that.rawCertificate;
    that.rawCertificate = nullptr;
    return *this;
}

const X509* Certificate::getRawCertificate() const {
    return rawCertificate;
}

std::string Certificate::getDistinguishedName() const {
    X509_NAME *subjectName = X509_get_subject_name(rawCertificate); // Must not be freed.

    if (!subjectName) {
        throw CryptoException(getOpenSslError());
    }

    char *buffer = X509_NAME_oneline(subjectName, nullptr, 0);

    if (!buffer) {
        throw CryptoException(getOpenSslError());
    }

    std::string distinguishedName(buffer);
    free(buffer);
    return distinguishedName;
}

const EVP_PKEY *Certificate::getPublicKey() const {
    // Using 'get0', EVP_PKEY must not be freed up after use.
    EVP_PKEY *publicKey = X509_get0_pubkey((X509*) rawCertificate);

    if (!publicKey) {
        throw CryptoException(getOpenSslError());
    }

    return publicKey;
}

}
