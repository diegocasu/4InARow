#ifndef INC_4INAROW_CONSTANTS_H
#define INC_4INAROW_CONSTANTS_H

#include <cstdint>
#include <string>

namespace fourinarow {

// Communication related quantities.
extern const unsigned short SERVER_PORT;
extern const unsigned short PLAYER_PORT;
extern const size_t BACKLOG_SIZE;
extern const unsigned long CLIENT_MATCHMAKING_TIMEOUT;

// File paths.
extern const std::string SERVER_CERTIFICATE_FOLDER;
extern const std::string SERVER_PLAYERS_FOLDER;
extern const std::string SERVER_PLAYER_KEY_SUFFIX;
extern const std::string CLIENT_CERTIFICATES_FOLDER;
extern const std::string CLIENT_KEYS_FOLDER;
extern const std::string CLIENT_PRIVATE_KEY_SUFFIX;

// Server distinguished name.
extern const std::string SERVER_DISTINGUISHED_NAME;

// Message types.
extern const uint8_t CLIENT_HELLO;
extern const uint8_t SERVER_HELLO;
extern const uint8_t PLAYER1_HELLO;
extern const uint8_t PLAYER2_HELLO;
extern const uint8_t END_HANDSHAKE;
extern const uint8_t REQ_PLAYER_LIST;
extern const uint8_t PLAYER_LIST;
extern const uint8_t CHALLENGE;
extern const uint8_t PLAYER_NOT_AVAILABLE;
extern const uint8_t PLAYER_ALREADY_CONNECTED;
extern const uint8_t CHALLENGE_REFUSED;
extern const uint8_t CHALLENGE_ACCEPTED;
extern const uint8_t PLAYER;
extern const uint8_t GOODBYE;
extern const uint8_t MOVE;
extern const uint8_t END_GAME;
extern const uint8_t PLAYER_NOT_REGISTERED;
extern const uint8_t PROTOCOL_VIOLATION;
extern const uint8_t MALFORMED_MESSAGE;
extern const uint8_t INTERNAL_ERROR;

// Size of message fields and cryptographic quantities, expressed in number of bytes.
extern const uint16_t MAX_MSG_SIZE;
extern const uint8_t MAX_IPV4_ADDRESS_SIZE;
extern const uint8_t NONCE_SIZE;
extern const uint8_t MAX_USERNAME_SIZE;
extern const uint8_t ECDH_PUBLIC_KEY_SIZE;
extern const uint16_t RSA_PUBLIC_KEY_SIZE;
extern const uint16_t DIGITAL_SIGNATURE_SIZE;
extern const uint16_t MAX_CERTIFICATE_SIZE;
extern const uint16_t MAX_PLAYER_LIST_SIZE;
extern const uint8_t KEY_SIZE;
extern const uint8_t IV_SIZE;
extern const uint8_t TAG_SIZE;

// Game quantities.
extern const uint8_t ROWS;
extern const uint8_t COLUMNS;

}

#endif //INC_4INAROW_CONSTANTS_H
