#ifndef INC_4INAROW_HANDLER_H
#define INC_4INAROW_HANDLER_H

#include <Message.h>
#include <Player.h>

namespace fourinarow {

/**
 * Base class for client handlers. It is used to store shared methods.
 */
class Handler {
    protected:
        /**
         * Performs the authenticated encryption of the given message,
         * returning the ciphertext concatenated with the tag.
         * @param message  the message to encrypt and authenticate.
         * @param player   the player to which the message will be sent.
         * @return         the ciphertext concatenated with the tag.
         * @throws SerializationException  if the message has not the expected format.
         * @throws CryptoException         if an error occurs while encrypting the message,
         *                                 or the maximum sequence number has been reached.
         */
        static std::vector<unsigned char> encryptAndAuthenticate(const Message *message, Player &player);

        /**
         * Performs the authenticated decryption of the given message, returning the plaintext.
         * @param message  the encrypted message.
         * @param player   the player receiving the message.
         * @return         the decrypted message.
         * @throws CryptoException  if an error occurs while decrypting the message,
         *                          or the tag is not valid,
         *                          or the maximum sequence number has been reached.
         */
        static std::vector<unsigned char> authenticateAndDecrypt(std::vector<unsigned char> &message, Player &player);
    public:
        Handler() = delete;
        ~Handler() = delete;
        Handler(const Handler&) = delete;
        Handler(Handler&&) = delete;
        Handler& operator=(const Handler&) = delete;
        Handler& operator=(Handler&&) = delete;
};

}

#endif //INC_4INAROW_HANDLER_H
