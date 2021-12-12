#include <string.h>
#include <arpa/inet.h>
#include <SerializationException.h>
#include <Utils.h>
#include "ServerHello.h"

namespace fourinarow {

ServerHello::ServerHello(std::vector<unsigned char> certificate,
                         std::vector<unsigned char> nonce,
                         std::vector<unsigned char> publicKey,
                         std::vector<unsigned char> digitalSignature)
    : certificate(std::move(certificate)),
      nonce(std::move(nonce)),
      publicKey(std::move(publicKey)),
      digitalSignature(std::move(digitalSignature)) {}

void ServerHello::checkIfSerializable() {
    if (certificate.empty() || certificate.size() > MAX_CERTIFICATE_SIZE) {
        throw SerializationException("The certificate size must be greater than zero, and less than or equal to " +
                                     std::to_string(MAX_CERTIFICATE_SIZE) +
                                     " bytes. Certificate size: " +
                                     std::to_string(certificate.size()) +
                                     " bytes");
    }

    if (nonce.size() != NONCE_SIZE) {
        throw SerializationException("The nonce size must be exactly " +
                                     std::to_string(NONCE_SIZE) +
                                     " bytes. Nonce size: " +
                                     std::to_string(nonce.size()) +
                                     " bytes");
    }

    if (publicKey.size() != PUBLIC_KEY_SIZE) {
        throw SerializationException("The public key size must be exactly " +
                                     std::to_string(PUBLIC_KEY_SIZE) +
                                     " bytes. Public key size: " +
                                     std::to_string(publicKey.size()) +
                                     " bytes");
    }

    if (digitalSignature.size() != DIGITAL_SIGNATURE_SIZE) {
        throw SerializationException("The digital signature size must be exactly " +
                                     std::to_string(DIGITAL_SIGNATURE_SIZE) +
                                     " bytes. Digital signature size: " +
                                     std::to_string(digitalSignature.size()) +
                                     " bytes");
    }
}

std::vector<unsigned char> ServerHello::serialize() {
    checkIfSerializable();

    size_t processedBytes = 0;
    size_t outputSize = sizeof(type) + sizeof(MAX_CERTIFICATE_SIZE) + certificate.size() +
                        nonce.size() + publicKey.size() + digitalSignature.size();
    std::vector<unsigned char> message(outputSize);

    // Serialize the type.
    memcpy(message.data(), &type, sizeof(type));
    processedBytes += sizeof(type);

    // Serialize the certificate and its length.
    uint16_t certificateLength = htons(certificate.size());
    memcpy(message.data() + processedBytes, &certificateLength, sizeof(certificateLength));
    processedBytes += sizeof(certificateLength);

    memcpy(message.data() + processedBytes, certificate.data(), certificate.size());
    processedBytes += certificate.size();

    // Serialize the nonce.
    memcpy(message.data() + processedBytes, nonce.data(), nonce.size());
    processedBytes += nonce.size();

    // Serialize the public key.
    memcpy(message.data() + processedBytes, publicKey.data(), publicKey.size());
    processedBytes += publicKey.size();

    // Serialize the digital signature.
    memcpy(message.data() + processedBytes, digitalSignature.data(), digitalSignature.size());

    return message;
}

void ServerHello::deserialize(const std::vector<unsigned char> &message) {
    size_t processedBytes = 0;

    // Check if the type matches the expected one.
    uint8_t receivedType;
    checkIfEnoughSpace(message, processedBytes, sizeof(receivedType));
    memcpy(&receivedType, message.data(), sizeof(receivedType));
    processedBytes += sizeof(receivedType);

    if (receivedType != SERVER_HELLO) {
        throw SerializationException("Malformed message");
    }

    // Deserialize the certificate and its length.
    uint16_t certificateLength;
    checkIfEnoughSpace(message, processedBytes, sizeof(certificateLength));
    memcpy(&certificateLength, message.data() + processedBytes, sizeof(certificateLength));
    certificateLength = ntohs(certificateLength);
    processedBytes += sizeof(certificateLength);

    if (certificateLength == 0) {
        throw SerializationException("Malformed message");
    }

    checkIfEnoughSpace(message, processedBytes, certificateLength);
    certificate.resize(certificateLength);
    memcpy(certificate.data(), message.data() + processedBytes, certificateLength);
    processedBytes += certificateLength;

    // Deserialize the nonce.
    checkIfEnoughSpace(message, processedBytes, NONCE_SIZE);
    nonce.resize(NONCE_SIZE);
    memcpy(nonce.data(), message.data() + processedBytes, NONCE_SIZE);
    processedBytes += NONCE_SIZE;

    // Deserialize the public key.
    checkIfEnoughSpace(message, processedBytes, PUBLIC_KEY_SIZE);
    publicKey.resize(PUBLIC_KEY_SIZE);
    memcpy(publicKey.data(), message.data() + processedBytes, PUBLIC_KEY_SIZE);
    processedBytes += PUBLIC_KEY_SIZE;

    // Deserialize the digital signature.
    checkIfEnoughSpace(message, processedBytes, DIGITAL_SIGNATURE_SIZE);
    digitalSignature.resize(DIGITAL_SIGNATURE_SIZE);
    memcpy(digitalSignature.data(), message.data() + processedBytes, DIGITAL_SIGNATURE_SIZE);
}

uint8_t ServerHello::getType() const {
    return type;
}

const std::vector<unsigned char>& ServerHello::getCertificate() const {
    return certificate;
}

const std::vector<unsigned char>& ServerHello::getNonce() const {
    return nonce;
}

const std::vector<unsigned char>& ServerHello::getPublicKey() const {
    return publicKey;
}

const std::vector<unsigned char> &ServerHello::getDigitalSignature() const {
    return digitalSignature;
}

}

std::ostream& operator<<(std::ostream &ostream, const fourinarow::ServerHello &serverHello) {
    ostream << "ServerHello{" << std::endl;
    ostream << "type=" << fourinarow::convertMessageType(serverHello.getType()) << ',' << std::endl;
    ostream << "certificate=" << std::endl << fourinarow::dumpVector(serverHello.getCertificate());
    ostream << "nonce=" << std::endl << fourinarow::dumpVector(serverHello.getNonce());
    ostream << "publicKey=" << std::endl << fourinarow::dumpVector(serverHello.getPublicKey());
    ostream << "digitalSignature=" << std::endl << fourinarow::dumpVector(serverHello.getDigitalSignature());
    ostream << '}';
    return ostream;
}
