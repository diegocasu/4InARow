#ifndef INC_4INAROW_CERTIFICATE_H
#define INC_4INAROW_CERTIFICATE_H

#include <string>
#include <vector>
#include <openssl/x509.h>

namespace fourinarow {

/**
 * Class representing a X509 certificate.
 */
class Certificate {
    private:
        const X509 *rawCertificate;
    public:
        /**
         * Creates a certificate starting from a raw pointer given by the OpenSSL API.
         * After a successful construction, the object assumes responsibility for freeing
         * the memory allocated by OpenSSL.
         * @param rawCertificate  the certificate.
         * @throws CryptoException  if the given pointer is a <code>nullptr</code>.
         */
        explicit Certificate(const X509 *rawCertificate);

        /**
         * Destroys the certificate and frees the memory reachable
         * by the raw pointer given at construction time.
         */
        ~Certificate();

        /**
         * Move constructs a certificate, automatically transferring the ownership of
         * the memory allocated by OpenSSL, so that the moved object cannot access it anymore.
         * @param that  the certificate to move.
         */
        Certificate(Certificate&& that) noexcept;

        Certificate(const Certificate&) = delete;
        Certificate& operator=(const Certificate&) = delete;
        Certificate& operator=(Certificate&&) = delete;

        const X509* getRawCertificate() const;

        /**
         * Returns a string containing the distinguished name saved in the certificate.
         * @return  the distinguished name.
         * @throws CryptoException  if an error occurred while retrieving the name.
         */
        std::string getDistinguishedName();

        /**
         * Returns the public key saved in the certificate, in a format suitable for the OpenSSL API.
         * @return  the public key.
         * @throws CryptoException  if an error occurred while retrieving the public key.
         */
        EVP_PKEY* getPublicKey();
};

}

#endif //INC_4INAROW_CERTIFICATE_H