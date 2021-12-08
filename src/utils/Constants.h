#ifndef INC_4INAROW_CONSTANTS_H
#define INC_4INAROW_CONSTANTS_H

#include <cstdint>

namespace fourinarow {

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
extern const uint8_t CHALLENGE_REFUSED;
extern const uint8_t CHALLENGE_ACCEPTED;
extern const uint8_t PLAYER;
extern const uint8_t GOODBYE;
extern const uint8_t MOVE;
extern const uint8_t END_GAME;
extern const uint8_t HEARTBEAT;
extern const uint8_t PROTOCOL_VIOLATION;
extern const uint8_t MALFORMED_MESSAGE;

// Size of message fields and cryptographic quantities, expressed in number of bytes.
extern const uint16_t MAX_MSG_SIZE;
extern const uint8_t NONCE_SIZE;
extern const uint8_t MAX_USERNAME_SIZE;
extern const uint8_t PUBLIC_KEY_SIZE;
extern const uint16_t DIGITAL_SIGNATURE_SIZE;
extern const uint8_t KEY_SIZE;
extern const uint8_t IV_SIZE;
extern const uint8_t TAG_SIZE;

}

#endif //INC_4INAROW_CONSTANTS_H
