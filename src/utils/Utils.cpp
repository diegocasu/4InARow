#include <openssl/bio.h>
#include <openssl/err.h>
#include "Constants.h"
#include "Utils.h"

namespace fourinarow {

std::string getOpenSslError() {
    BIO *bio = BIO_new(BIO_s_mem());
    ERR_print_errors(bio);

    char *buffer;
    size_t length = BIO_get_mem_data(bio, &buffer);
    std::string error(buffer, length);

    BIO_free(bio);
    return error;
}

std::string dumpVector(const std::vector<unsigned char> &vector) {
    BIO *bio = BIO_new(BIO_s_mem());
    BIO_dump(bio, (const char*) vector.data(), vector.size());

    char *buffer;
    size_t length = BIO_get_mem_data(bio, &buffer);
    std::string dump(buffer, length);

    BIO_free(bio);
    return dump;
}

void cleanse(std::vector<unsigned char> &vector) {
    OPENSSL_cleanse(vector.data(), vector.size());
}

std::string convertMessageType(uint8_t messageType) {
    if (messageType == CLIENT_HELLO)         return "CLIENT_HELLO";
    if (messageType == SERVER_HELLO)         return "SERVER_HELLO";
    if (messageType == PLAYER1_HELLO)        return "PLAYER1_HELLO";
    if (messageType == PLAYER2_HELLO)        return "PLAYER2_HELLO";
    if (messageType == END_HANDSHAKE)        return "END_HANDSHAKE";
    if (messageType == REQ_PLAYER_LIST)      return "REQ_PLAYER_LIST";
    if (messageType == PLAYER_LIST)          return "PLAYER_LIST";
    if (messageType == CHALLENGE)            return "CHALLENGE";
    if (messageType == PLAYER_NOT_AVAILABLE) return "PLAYER_NOT_AVAILABLE";
    if (messageType == CHALLENGE_REFUSED)    return "CHALLENGE_REFUSED";
    if (messageType == CHALLENGE_ACCEPTED)   return "CHALLENGE_ACCEPTED";
    if (messageType == PLAYER)               return "PLAYER";
    if (messageType == GOODBYE)              return "GOODBYE";
    if (messageType == MOVE)                 return "MOVE";
    if (messageType == END_GAME)             return "END_GAME";
    if (messageType == HEARTBEAT)            return "HEARTBEAT";
    if (messageType == PROTOCOL_VIOLATION)   return "PROTOCOL_VIOLATION";
    if (messageType == MALFORMED_MESSAGE)    return "MALFORMED_MESSAGE";
    else                                     return "CURRENTLY_NOT_SUPPORTED_TYPE";
}

}
