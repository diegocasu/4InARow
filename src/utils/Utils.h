#ifndef INC_4INAROW_UTILS_H
#define INC_4INAROW_UTILS_H

#include <string>
#include <vector>
#include <openssl/pem.h>

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
 * Translates a message type code into a human readable string
 * containing the type itself.
 * @param messageType  the message type code.
 * @return  the string containing the human readable type.
 */
std::string convertMessageType(uint8_t messageType);

}

#endif //INC_4INAROW_UTILS_H
