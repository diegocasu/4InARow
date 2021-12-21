#ifndef INC_4INAROW_CERTIFICATESTORE_H
#define INC_4INAROW_CERTIFICATESTORE_H

#include <string>
#include <vector>
#include <openssl/x509.h>
#include "Certificate.h"

namespace fourinarow {

/**
 * Class representing a certificate store holding trusted certificates.
 * It allows to add certificates and certificate revocation lists, and
 * to verify untrusted ones against them. Moreover, it offers methods
 * to serialize and deserialize certificates using the DER format.
 * The store assumes an X509 format for all the items.
 */
class CertificateStore {
    private:
        X509_STORE *store;
        bool empty;

        /**
         * Loads a certificate from a file. The certificate must be saved in PEM format.
         * It is responsibility of the caller to free the memory allocated to hold the certificate.
         * @param path  the file path.
         * @return      the certificate.
         * @throws CryptoException  if the file cannot be opened or the certificate cannot be loaded.
         */
        static X509* loadCertificate(const std::string &path);

        /**
         * Loads a certificate revocation list from a file. The list must be saved in PEM format.
         * It is responsibility of the caller to free the memory allocated to hold
         * the certificate revocation list.
         * @param path  the file path.
         * @return      the certificate revocation list.
         * @throws CryptoException  if the file cannot be opened or the list cannot be loaded.
         */
        static X509_CRL* loadCertificateRevocationList(const std::string &path);
    public:
        /**
         * Creates an empty certificate store.
         * @throws CryptoException  if an error occurs while initializing the store.
         */
        CertificateStore();

        /**
         * Destroys a certificate store and its items.
         */
        ~CertificateStore();

        /**
         * Move constructs a certificate store, automatically transferring the ownership of
         * the certificates, so that the moved object cannot access them anymore.
         * Calling <code>addCertificate(), addCertificateRevocationList()</code> or
         * <code>verifyCertificate()</code> on the moved object results in undefined behaviour.
         * @param that  the certificate store to move.
         */
        CertificateStore(CertificateStore &&that) noexcept;

        /**
         * Move assigns a certificate store, automatically transferring the ownership of
         * the certificates, so that the moved object cannot access them anymore.
         * Calling <code>addCertificate(), addCertificateRevocationList()</code> or
         * <code>verifyCertificate()</code> on the moved object results in undefined behaviour.
         * @param that  the certificate store to move.
         */
        CertificateStore& operator=(CertificateStore &&that) noexcept;

        CertificateStore(const CertificateStore&) = delete;
        CertificateStore& operator=(const CertificateStore&) = delete;

        /**
         * Adds a trusted certificate to the store, loading it from a file.
         * The certificate must be saved in PEM format.
         * @param path  the file path.
         * @throws CryptoException  if the file cannot be opened or the certificate cannot be loaded.
         */
        void addCertificate(const std::string &path);

        /**
         * Adds a trusted certificate revocation lists to the store, loading it
         * from a file. The list must be saved in PEM format.
         * @param path  the file path.
         * @throws CryptoException  if the file cannot be opened or the list cannot be loaded.
         */
        void addCertificateRevocationList(const std::string &path);

        /**
         * Verifies an untrusted certificate against the trusted ones saved in the store.
         * @param certificate  the untrusted certificate.
         * @return             true if the certificate is valid, false otherwise.
         * @throws CryptoException  if the store has no loaded certificates, or an error
         *                          occurs while verifying the certificate.
         */
        bool verifyCertificate(const Certificate &certificate) const;

        /**
         * Serializes a certificate in DER format. The certificate must be saved in PEM format in a file.
         * @param path  the file path.
         * @return      the serialized certificate.
         * @throws CryptoException         if the file cannot be opened or the certificate cannot be loaded.
         * @throws SerializationException  if an error occurs while serializing the certificate.
         */
        static std::vector<unsigned char> serializeCertificate(const std::string &path);

        /**
         * Deserializes a certificate in DER format.
         * @param serializedCertificate  the certificate in DER format.
         * @return                       a certificate object.
         * @throws SerializationException  if the certificate is empty, or the certificate is not
         *                                 represented in a correct DER format.
         */
        static Certificate deserializeCertificate(const std::vector<unsigned char> &serializedCertificate);
};

}

#endif //INC_4INAROW_CERTIFICATESTORE_H
