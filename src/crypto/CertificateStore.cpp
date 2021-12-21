#include <string.h>
#include <openssl/pem.h>
#include <CryptoException.h>
#include <SerializationException.h>
#include <Utils.h>
#include "CertificateStore.h"

namespace fourinarow {

CertificateStore::CertificateStore() {
    store = X509_STORE_new();

    if (!store) {
        throw CryptoException(getOpenSslError());
    }

    empty = true;
}

CertificateStore::~CertificateStore() {
    if (store != nullptr) {
        X509_STORE_free(store);
    }
}

CertificateStore::CertificateStore(CertificateStore &&that) noexcept : store(that.store), empty(that.empty) {
    that.store = nullptr; // Avoid a call to X509_STORE_free() when destructing "that".
    that.empty = true;
}

CertificateStore& CertificateStore::operator=(CertificateStore &&that) noexcept {
    if (store != nullptr) {
        X509_STORE_free(store);
    }

    store = that.store;
    empty = that.empty;
    that.store = nullptr; // Avoid a call to X509_STORE_free() when destructing "that".
    that.empty = true;
    return *this;
}

X509* CertificateStore::loadCertificate(const std::string &path) {
    FILE *file = fopen(path.data(), "r");

    if (!file) {
        throw CryptoException("Impossible to open the certificate file");
    }

    X509 *certificate = PEM_read_X509(file, nullptr, nullptr, nullptr);
    fclose(file);

    if (!certificate) {
        throw CryptoException(getOpenSslError());
    }

    return certificate;
}

void CertificateStore::addCertificate(const std::string &path) {
    X509 *certificate = loadCertificate(path);

    if (1 != X509_STORE_add_cert(store, certificate)) {
        X509_free(certificate);
        throw CryptoException(getOpenSslError());
    }

    X509_free(certificate);
    empty = false;
}

X509_CRL* CertificateStore::loadCertificateRevocationList(const std::string &path) {
    FILE *file = fopen(path.data(), "r");

    if (!file) {
        throw CryptoException("Impossible to open the certificate revocation list file");
    }

    X509_CRL *certificateRevocationList = PEM_read_X509_CRL(file, nullptr, nullptr, nullptr);
    fclose(file);

    if (!certificateRevocationList) {
        throw CryptoException(getOpenSslError());
    }

    return certificateRevocationList;
}

void CertificateStore::addCertificateRevocationList(const std::string &path) {
    X509_CRL *certificateRevocationList = loadCertificateRevocationList(path);

    if (1 != X509_STORE_add_crl(store, certificateRevocationList)) {
        X509_CRL_free(certificateRevocationList);
        throw CryptoException(getOpenSslError());
    }

    X509_CRL_free(certificateRevocationList);

    if (1 != X509_STORE_set_flags(store, X509_V_FLAG_CRL_CHECK)) {
        throw CryptoException(getOpenSslError());
    }
}

bool CertificateStore::verifyCertificate(const Certificate &certificate) const {
    if (empty) {
        throw CryptoException("No loaded certificates in the store");
    }

    X509_STORE_CTX *context = X509_STORE_CTX_new();

    if (!context) {
        throw CryptoException(getOpenSslError());
    }

    if (1 != X509_STORE_CTX_init(context, store, (X509*) certificate.getRawCertificate(), nullptr)) {
        X509_STORE_CTX_free(context);
        throw CryptoException(getOpenSslError());
    }

    auto result = X509_verify_cert(context);
    X509_STORE_CTX_free(context);

    if (result < 0) {
        throw CryptoException(getOpenSslError());
    }

    if (result == 0) {
        return false;
    }
    return true;
}

std::vector<unsigned char> CertificateStore::serializeCertificate(const std::string &path) {
    X509 *certificate = loadCertificate(path);
    unsigned char *certificateBuffer = nullptr;
    auto outputSize = i2d_X509(certificate, &certificateBuffer);

    if (outputSize < 0) {
        X509_free(certificate);
        throw SerializationException(getOpenSslError());
    }

    std::vector<unsigned char> serializedCertificate(outputSize);
    memcpy(serializedCertificate.data(), certificateBuffer, outputSize);

    X509_free(certificate);
    OPENSSL_free(certificateBuffer);
    return serializedCertificate;
}

Certificate CertificateStore::deserializeCertificate(const std::vector<unsigned char> &serializedCertificate) {
    if (serializedCertificate.empty()) {
        throw SerializationException("Empty certificate");
    }

    const unsigned char *buffer = serializedCertificate.data();
    X509 *certificate = d2i_X509(nullptr, &buffer, serializedCertificate.size());

    if (!certificate) {
        throw SerializationException(getOpenSslError());
    }

    return Certificate(certificate);
}

}
