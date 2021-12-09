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
const uint8_t MAX_IPV4_ADDRESS_SIZE   = 15;
const uint8_t NONCE_SIZE              = 4;
const uint8_t MAX_USERNAME_SIZE       = 255;
const uint8_t PUBLIC_KEY_SIZE         = 65;                       // Elliptic-curve Diffie-Hellman with prime256v1 curve.
const uint16_t DIGITAL_SIGNATURE_SIZE = 256;                      // RSA-2048 digital signatures.
const uint8_t KEY_SIZE                = 16;                       // AES-128 GCM
const uint8_t IV_SIZE                 = 12;                       // AES-128 GCM
const uint8_t TAG_SIZE                = 16;                       // AES-128 GCM

const uint16_t MAX_CERTIFICATE_SIZE   = MAX_MSG_SIZE -            // Size derived from the composition of SERVER_HELLO.
                                        sizeof(uint8_t) -         // sizeof(uint8_t) refers to the "type" field size,
                                        NONCE_SIZE -              // while sizeof(uint16_t) refers to the certificate
                                        PUBLIC_KEY_SIZE -         // length sent in the serialized message.
                                        DIGITAL_SIGNATURE_SIZE -
                                        sizeof(uint16_t);

const uint16_t MAX_PLAYER_LIST_SIZE   = MAX_MSG_SIZE -            // Size derived from the composition of PLAYER_LIST.
                                        sizeof(uint8_t) -         // sizeof(uint8_t) refers to the "type" field size,
                                        sizeof(uint16_t);         // while sizeof(uint16_t) refers to the list length
                                                                  // sent in the serialized message.
}
