#include "Constants.h"

namespace fourinarow {

const unsigned short SERVER_PORT               = 5000;
const unsigned short PLAYER_PORT               = 5001;
const size_t BACKLOG_SIZE                      = 100;
const unsigned long CLIENT_MATCHMAKING_TIMEOUT = 30;                       // In seconds.
const unsigned int P2P_MAX_CONNECTION_RETRIES  = 3;                        // Each retry is interleaved by 1 second of sleep.
const unsigned int MAX_TURN_DURATION           = 90;                       // In seconds.

const std::string SERVER_CERTIFICATE_FOLDER    = "./certificate/";
const std::string SERVER_PLAYERS_FOLDER        = "./players/";
const std::string SERVER_PLAYER_KEY_SUFFIX     = "_pubkey.pem";
const std::string CLIENT_CERTIFICATES_FOLDER   = "./certificates/";
const std::string CLIENT_KEYS_FOLDER           = "./keys/";
const std::string CLIENT_PRIVATE_KEY_SUFFIX    = "_privkey.pem";

const std::string SERVER_DISTINGUISHED_NAME    = "/C=IT/O=Unipi/OU=Cybersecurity/CN=4InARow";

const uint8_t CLIENT_HELLO                     = 1;
const uint8_t SERVER_HELLO                     = 2;
const uint8_t PLAYER1_HELLO                    = 3;
const uint8_t PLAYER2_HELLO                    = 4;
const uint8_t END_HANDSHAKE                    = 5;
const uint8_t REQ_PLAYER_LIST                  = 6;
const uint8_t PLAYER_LIST                      = 7;
const uint8_t CHALLENGE                        = 8;
const uint8_t PLAYER_NOT_AVAILABLE             = 9;
const uint8_t PLAYER_ALREADY_CONNECTED         = 10;
const uint8_t CHALLENGE_REFUSED                = 11;
const uint8_t CHALLENGE_ACCEPTED               = 12;
const uint8_t PLAYER                           = 13;
const uint8_t GOODBYE                          = 14;
const uint8_t MOVE                             = 15;
const uint8_t END_GAME                         = 16;
const uint8_t PLAYER_NOT_REGISTERED            = 17;
const uint8_t PROTOCOL_VIOLATION               = 18;
const uint8_t MALFORMED_MESSAGE                = 19;
const uint8_t INTERNAL_ERROR                   = 20;

const uint16_t MAX_MSG_SIZE                    = 65535;
const uint8_t MAX_IPV4_ADDRESS_SIZE            = 15;
const uint8_t NONCE_SIZE                       = 4;
const uint8_t MAX_USERNAME_SIZE                = 255;
const uint8_t ECDH_PUBLIC_KEY_SIZE             = 91;                       // ECDH with prime256v1 curve, DER format.
const uint16_t RSA_PUBLIC_KEY_SIZE             = 294;                      // RSA-2048, DER format.
const uint16_t DIGITAL_SIGNATURE_SIZE          = 256;                      // RSA-2048 digital signatures.
const uint8_t KEY_SIZE                         = 16;                       // AES-128 GCM.
const uint8_t IV_SIZE                          = 12;                       // AES-128 GCM.
const uint8_t TAG_SIZE                         = 16;                       // AES-128 GCM.

const uint16_t MAX_CERTIFICATE_SIZE            = MAX_MSG_SIZE -            // Size derived from the composition of SERVER_HELLO.
                                                 sizeof(uint8_t) -         // sizeof(uint8_t) refers to the "type" field size,
                                                 NONCE_SIZE -              // while sizeof(uint16_t) refers to the certificate
                                                 ECDH_PUBLIC_KEY_SIZE -    // length sent in the serialized message.
                                                 DIGITAL_SIGNATURE_SIZE -
                                                 sizeof(uint16_t);

const uint16_t MAX_PLAYER_LIST_SIZE            = MAX_MSG_SIZE -            // Size derived from the composition of PLAYER_LIST.
                                                 sizeof(uint8_t) -         // sizeof(uint8_t) refers to the "type" field size, while sizeof(uint16_t)
                                                 sizeof(uint16_t);         // refers to the list length sent in the serialized message.

const uint8_t ROWS                             = 6;
const uint8_t COLUMNS                          = 7;

}
