#ifndef INC_4INAROW_UTILS_H
#define INC_4INAROW_UTILS_H

#include <string>
#include <vector>
#include <regex>
#include "Constants.h"
#include "Player.h"

namespace fourinarow {

/**
 * Returns a string containing a human readable description of the error
 * that occurred while using the OpenSSL API.
 * @return  the string containing the error.
 */
std::string getOpenSslError();

/**
 * Returns an indented string containing the hexadecimal representation of the
 * bytes held by a given array.
 * @param vector  the array of bytes.
 * @return  the string containing the hexadecimal representation of the bytes.
 */
std::string dumpVector(const std::vector<unsigned char> &vector);

/**
 * Fills the given array with zeros destroying its content, so that
 * the compiler does not remove the operations when optimizing.
 * It relies on <code>OPENSSL_cleanse()</code>.
 * @param vector  the array whose content must be destroyed.
 */
void cleanse(std::vector<unsigned char> &vector);

/**
 * Fills the given string with zeros destroying its content, so that
 * the compiler does not remove the operations when optimizing.
 * It relies on <code>OPENSSL_cleanse()</code>.
 * @param string  the string whose content must be destroyed.
 */
void cleanse(std::string &string);

/**
 * Fills the given 8-bit unsigned integer with zeros destroying its content,
 * so that the compiler does not remove the operations when optimizing.
 * It relies on <code>OPENSSL_cleanse()</code>.
 * @param integer  the 8-bit unsigned integer whose content must be destroyed.
 */
void cleanse(uint8_t &integer);

/**
 * Fills the given boolean with zeros destroying its content, so that
 * the compiler does not remove the operations when optimizing.
 * It relies on <code>OPENSSL_cleanse()</code>.
 * @param boolean  the boolean whose content must be destroyed.
 */
void cleanse(bool &boolean);

/**
 * Translates a message type code into a human readable string
 * containing the type itself.
 * @param messageType  the message type code.
 * @return  the string containing the human readable type.
 */
std::string convertMessageType(const uint8_t &messageType);

/**
 * Translates a player status into a human readable string
 * containing the status itself.
 * @param status  the player status.
 * @return  the string containing the human readable status.
 */
std::string convertClientStatus(const Player::Status &status);

/**
 * Returns the type of a given binary message.
 * @tparam Exception  the exception type.
 * @param message     the binary message.
 * @return            the type of the message.
 * @throws Exception  if the message is not big enough to hold a type field.
 */
template<typename Exception>
uint8_t getMessageType(const std::vector<unsigned char> &message) {
    uint8_t type;

    if (message.size() < sizeof(type)) {
        throw Exception("Malformed message");
    }

    memcpy(&type, message.data(), sizeof(type));
    return type;
}

/**
 * Checks if the given key is correctly sized.
 * If the check fails, the function throws a user specified exception.
 * @tparam Exception  the exception type.
 * @param key         the key.
 * @throws Exception  if the key is wrongly sized.
 */
template<typename Exception>
void checkKeySize(const std::vector<unsigned char> &key) {
    if (key.size() != KEY_SIZE) {
        throw Exception("The key size must be exactly " +
                        std::to_string(KEY_SIZE) +
                        " bytes. Key size: " +
                        std::to_string(key.size()) +
                        " bytes");
    }
}

/**
 * Checks if the given initialization vector is correctly sized.
 * If the check fails, the function throws a user specified exception.
 * @tparam Exception  the exception type.
 * @param iv          the initialization vector.
 * @throws Exception  if the initialization vector is wrongly sized.
 */
template<typename Exception>
void checkIvSize(const std::vector<unsigned char> &iv) {
    if (iv.size() != IV_SIZE) {
        throw Exception("The IV size must be exactly " +
                        std::to_string(IV_SIZE) +
                        " bytes. IV size: " +
                        std::to_string(iv.size()) +
                        " bytes");
    }
}

/**
 * Checks if the given username is valid.
 * If the check fails, the function throws a user specified exception.
 * @tparam Exception  the exception type.
 * @param username    the username.
 * @throws Exception  if the username is invalid.
 */
template<typename Exception>
void checkUsernameValidity(const std::string &username) {
    if (username.empty() || username.size() > MAX_USERNAME_SIZE || std::regex_search(username, std::regex("[^A-Za-z0-9]"))) {
        throw Exception("The username must be composed of at least 1 character, at most " +
                        std::to_string(MAX_USERNAME_SIZE) +
                        " characters and cannot contain whitespaces or special characters. Username: " +
                        username);
    }
}

/**
 * Checks if the given nonce is correctly sized.
 * If the check fails, the function throws a user specified exception.
 * @tparam Exception  the exception type.
 * @param nonce       the nonce.
 * @throws Exception  if the nonce is wrongly sized.
 */
template<typename Exception>
void checkNonceSize(const std::vector<unsigned char> &nonce) {
    if (nonce.size() != NONCE_SIZE) {
        throw Exception("The nonce size must be exactly " +
                        std::to_string(NONCE_SIZE) +
                        " bytes. Nonce size: " +
                        std::to_string(nonce.size()) +
                        " bytes");
    }
}

/**
 * Checks if the given Elliptic-curve Diffie-Hellman public key
 * is correctly sized. If the check fails, the function throws
 * a user specified exception.
 * @tparam Exception  the exception type.
 * @param publicKey   the public key.
 * @throws Exception  if the public key is wrongly sized.
 */
template<typename Exception>
void checkEcdhPublicKeySize(const std::vector<unsigned char> &publicKey) {
    if (publicKey.size() != ECDH_PUBLIC_KEY_SIZE) {
        throw Exception("The ECDH public key size must be exactly " +
                        std::to_string(ECDH_PUBLIC_KEY_SIZE) +
                        " bytes. Public key size: " +
                        std::to_string(publicKey.size()) +
                        " bytes");
    }
}

/**
 * Checks if the given RSA public key is correctly sized.
 * If the check fails, the function throws a user specified exception.
 * @tparam Exception  the exception type.
 * @param publicKey   the public key.
 * @throws Exception  if the public key is wrongly sized.
 */
template<typename Exception>
void checkRsaPublicKeySize(const std::vector<unsigned char> &publicKey) {
    if (publicKey.size() != RSA_PUBLIC_KEY_SIZE) {
        throw Exception("The RSA public key size must be exactly " +
                        std::to_string(RSA_PUBLIC_KEY_SIZE) +
                        " bytes. Public key size: " +
                        std::to_string(publicKey.size()) +
                        " bytes");
    }
}

/**
 * Checks if the given digital signature is correctly sized.
 * If the check fails, the function throws a user specified exception.
 * @tparam Exception        the exception type.
 * @param digitalSignature  the digital signature.
 * @throws Exception  if the digital signature is wrongly sized.
 */
template<typename Exception>
void checkDigitalSignatureSize(const std::vector<unsigned char> &digitalSignature) {
    if (digitalSignature.size() != DIGITAL_SIGNATURE_SIZE) {
        throw Exception("The digital signature size must be exactly " +
                        std::to_string(DIGITAL_SIGNATURE_SIZE) +
                        " bytes. Digital signature size: " +
                        std::to_string(digitalSignature.size()) +
                        " bytes");
    }
}

/**
 * Checks if the given column index is valid.
 * If the check fails, the function throws a user specified exception.
 * @tparam Exception   the exception type.
 * @param columnIndex  the column index.
 * @throws Exception  if the column index is invalid.
 */
template<typename Exception>
void checkColumnIndexValidity(uint8_t columnIndex) {
    if (columnIndex >= COLUMNS) {
        throw Exception("The column index must be a number between 0 and " +
                        std::to_string(COLUMNS - 1) +
                        ". Column index: " +
                        std::to_string(unsigned(columnIndex)));
    }
}

/**
 * Checks if the given player list is correctly sized.
 * If the check fails, the function throws a user specified exception.
 * @tparam Exception  the exception type.
 * @param playerList  the player list.
 * @throws Exception  if the player list is wrongly sized.
 */
template<typename Exception>
void checkPlayerListSize(const std::string &playerList) {
    if (playerList.size() > MAX_PLAYER_LIST_SIZE) {
        throw Exception("The player list size must be less than or equal to " +
                        std::to_string(MAX_PLAYER_LIST_SIZE) +
                        " bytes. Player list size: " +
                        std::to_string(playerList.size()) +
                        " bytes");
    }
}

/**
 * Checks if the given certificate is correctly sized.
 * If the check fails, the function throws a user specified exception.
 * @tparam Exception   the exception type.
 * @param certificate  the certificate.
 * @throws Exception  if the certificate is wrongly sized.
 */
template<typename Exception>
void checkCertificateSize(const std::vector<unsigned char> &certificate) {
    if (certificate.empty() || certificate.size() > MAX_CERTIFICATE_SIZE) {
        throw Exception("The certificate size must be greater than zero, and less than or equal to " +
                        std::to_string(MAX_CERTIFICATE_SIZE) +
                        " bytes. Certificate size: " +
                        std::to_string(certificate.size()) +
                        " bytes");
    }
}

/**
 * Concatenate two vectors. The concatenation is obtained by copying the
 * elements of the source vector into the destination one.
 * This function is used as the base case for the variadic <code>concatenate()</code>.
 * @tparam T           the type of the vector's elements.
 * @param destination  the destination vector. It will be automatically resized, if needed.
 * @param vector       the vector to concatenate.
 */
template<typename T>
void concatenate(std::vector<T> &destination, const std::vector<T> &vector) {
    destination.insert(destination.end(), vector.begin(), vector.end());
}

/**
 * Concatenate multiple vectors in the order they are passed.
 * The concatenation is obtained by copying the elements of
 * the source vectors into a destination vector.
 * @tparam T           the type of the vector's elements.
 * @tparam Args        the type of the elements to concatenate.
 *                     It must be <code>std::vector\<T\></code>.
 * @param destination  the destination vector. It will be automatically resized, if needed.
 * @param vector       the vector to concatenate.
 * @param args         one or more other vectors to concatenate.
 */
template<typename T, typename ...Args>
void concatenate(std::vector<T> &destination, const std::vector<T> &vector, const Args&... args) {
    destination.insert(destination.end(), vector.begin(), vector.end());
    concatenate(destination, args...);
}

}

#endif //INC_4INAROW_UTILS_H
