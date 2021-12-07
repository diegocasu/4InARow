#include "Constants.h"

namespace fourinarow {

const uint8_t CLIENT_HELLO = 1;
const uint8_t SERVER_HELLO = 2;
const uint8_t PLAYER_HELLO = 3;
const uint8_t END_HANDSHAKE = 4;
const uint8_t REQ_PLAYER_LIST = 5;
const uint8_t PLAYER_LIST = 6;
const uint8_t CHALLENGE = 7;
const uint8_t PLAYER_NOT_AVAILABLE = 8;
const uint8_t CHALLENGE_REFUSED = 9;
const uint8_t CHALLENGE_ACCEPTED = 10;
const uint8_t PLAYER = 11;
const uint8_t GOODBYE = 12;
const uint8_t MOVE = 13;
const uint8_t END_GAME = 14;
const uint8_t HEARTBEAT = 15;
const uint8_t PROTOCOL_VIOLATION = 16;
const uint8_t MALFORMED_MESSAGE = 17;

const uint8_t NONCE_SIZE = 4;
const uint8_t MAX_USERNAME_SIZE = 255;
const uint8_t KEY_SIZE = 16;
const uint8_t IV_SIZE = 12;
const uint8_t AES_GCM_TAG_SIZE = 16;

}
