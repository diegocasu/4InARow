#include "Constants.h"

namespace fourinarow {

const uint8_t CLIENT_HELLO            = 1;
const uint8_t SERVER_HELLO            = 2;
const uint8_t PLAYER1_HELLO           = 3;
const uint8_t PLAYER2_HELLO           = 4;
const uint8_t END_HANDSHAKE           = 5;
const uint8_t REQ_PLAYER_LIST         = 6;
const uint8_t PLAYER_LIST             = 7;
const uint8_t CHALLENGE               = 8;
const uint8_t PLAYER_NOT_AVAILABLE    = 9;
const uint8_t CHALLENGE_REFUSED       = 10;
const uint8_t CHALLENGE_ACCEPTED      = 11;
const uint8_t PLAYER                  = 12;
const uint8_t GOODBYE                 = 13;
const uint8_t MOVE                    = 14;
const uint8_t END_GAME                = 15;
const uint8_t HEARTBEAT               = 16;
const uint8_t PROTOCOL_VIOLATION      = 17;
const uint8_t MALFORMED_MESSAGE       = 18;

const uint16_t MAX_MSG_SIZE           = 65535;
const uint8_t NONCE_SIZE              = 4;
const uint8_t MAX_USERNAME_SIZE       = 255;
const uint8_t PUBLIC_KEY_SIZE         = 65;                       // Elliptic-curve Diffie-Hellman with prime256v1 curve.
const uint16_t DIGITAL_SIGNATURE_SIZE = 256;                      // RSA-2048 digital signatures.
const uint8_t KEY_SIZE                = 16;                       // AES-128 GCM
const uint8_t IV_SIZE                 = 12;                       // AES-128 GCM
const uint8_t TAG_SIZE                = 16;                       // AES-128 GCM

const uint16_t MAX_CERTIFICATE_SIZE   = MAX_MSG_SIZE -
                                        NONCE_SIZE -
                                        PUBLIC_KEY_SIZE -
                                        DIGITAL_SIGNATURE_SIZE -
                                        sizeof(uint16_t);         // Size derived from the composition of SERVER_HELLO.
}
