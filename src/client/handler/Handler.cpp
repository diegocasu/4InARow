#include <iostream>
#include <string.h>
#include <arpa/inet.h>
#include "Handler.h"

namespace fourinarow {

std::vector<unsigned char> Handler::encryptAndAuthenticate(const Message *message, Player &player) {
    // Generate the additional authenticated data using the sequence number.
    uint32_t sequenceNumber = htonl(player.getSequenceNumber());
    std::vector<unsigned char> aad(sizeof(sequenceNumber));
    memcpy(aad.data(), &sequenceNumber, sizeof(sequenceNumber));

    auto authenticatedCiphertext = player.getCipher().encrypt(message->serialize(), aad);
    player.incrementSequenceNumber();

    return authenticatedCiphertext;
}

std::vector<unsigned char> Handler::authenticateAndDecrypt(std::vector<unsigned char> &message, Player &player) {
    // Generate the additional authenticated data using the sequence number.
    uint32_t sequenceNumber = htonl(player.getSequenceNumber());
    std::vector<unsigned char> aad(sizeof(sequenceNumber));
    memcpy(aad.data(), &sequenceNumber, sizeof(sequenceNumber));

    auto plaintext = player.getCipher().decrypt(message, aad);
    player.incrementSequenceNumber();

    return plaintext;
}

}
