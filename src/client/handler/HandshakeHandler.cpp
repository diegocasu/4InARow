#include <iostream>
#include <Utils.h>
#include <SerializationException.h>
#include <CryptoException.h>
#include <ClientHello.h>
#include <ServerHello.h>
#include <EndHandshake.h>
#include <PlayerListMessage.h>
#include <DigitalSignature.h>
#include "HandshakeHandler.h"

namespace fourinarow {

void HandshakeHandler::sendClientHello(const TcpSocket &socket, Player &myselfForServer, const std::string &username) {
    std::cout << "Handshake: sending a CLIENT_HELLO message" << std::endl;

    try {
        myselfForServer.setUsername(username);
        myselfForServer.generateClientNonce();
        myselfForServer.generateClientKeys();

        socket.send(ClientHello(username,
                                        myselfForServer.getClientNonce(),
                                        myselfForServer.getClientPublicKey()
                                        ).serialize());
    }  catch (const std::exception &exception) {
        std::cerr << "Impossible to send the CLIENT_HELLO message: " << exception.what() << std::endl;
        throw std::runtime_error("Handshake with the server failed");
    }
}

bool HandshakeHandler::isValidCertificate(const CertificateStore &certificateStore,
                                          const Certificate &serverCertificate) {
    return certificateStore.verifyCertificate(serverCertificate) &&
           (serverCertificate.getDistinguishedName() == SERVER_DISTINGUISHED_NAME);
}

void HandshakeHandler::receiveServerHello(const TcpSocket &socket, Player &myselfForServer,
                                          const CertificateStore &certificateStore) {
    std::cout << "Handshake: waiting for a SERVER_HELLO message" << std::endl;

    try {
        auto message = socket.receive();
        auto type = getMessageType<SerializationException>(message);

        if (type != SERVER_HELLO) {
            throw SerializationException(convertMessageType(type));
        }

        ServerHello serverHello;
        serverHello.deserialize(message);
        auto serverCertificate = CertificateStore::deserializeCertificate(serverHello.getCertificate());

        if (!isValidCertificate(certificateStore, serverCertificate)) {
            throw CryptoException("Invalid server certificate");
        }

        myselfForServer.setServerNonce(serverHello.getNonce());
        myselfForServer.setServerPublicKey(serverHello.getPublicKey());
        myselfForServer.generateFreshnessProof(serverHello.getCertificate());

        if (!DigitalSignature::verify(myselfForServer.getFreshnessProof(),
                                      serverHello.getDigitalSignature(),
                                      serverCertificate.getPublicKey())) {
            throw CryptoException("Invalid signature of the freshness proof");
        }
    } catch (const std::exception &exception) {
        std::cerr << "Impossible to continue the handshake: " << exception.what() << std::endl;
        throw std::runtime_error("Handshake with the server failed");
    }
}

std::string HandshakeHandler::endHandshake(const TcpSocket &socket, Player &myselfForServer,
                                           const DigitalSignature &digitalSignature) {
    std::cout << "Handshake: sending an END_HANDSHAKE message" << std::endl;

    try {
        auto signature = digitalSignature.sign(myselfForServer.getFreshnessProof());
        EndHandshake endHandshake(signature);
        socket.send(endHandshake.serialize());

        // The handshake could fail or succeed: depending on the case, the response of the server is encrypted or not.
        auto serverResponse = socket.receive();
        auto type = getMessageType<SerializationException>(serverResponse);

        if (type == PROTOCOL_VIOLATION || type == MALFORMED_MESSAGE || type == INTERNAL_ERROR) {
            // Handshake failed.
            throw std::runtime_error(convertMessageType(type));
        }

        myselfForServer.initCipher();

        // The handshake should have succeeded and the response should be encrypted.
        auto message = authenticateAndDecrypt(serverResponse, myselfForServer);
        type = getMessageType<SerializationException>(message);

        if (type != PLAYER_LIST) {
            throw SerializationException(convertMessageType(type));
        }

        PlayerListMessage playerListMessage;
        playerListMessage.deserialize(message);
        return playerListMessage.getPlayerList();
    } catch (const std::exception &exception) {
        std::cerr << "Impossible to finalize the handshake: " << exception.what() << std::endl;
        throw std::runtime_error("Handshake with the server failed");
    }
}

std::string HandshakeHandler::doHandshakeWithServer(const TcpSocket &socket,
                                                    Player &myselfForServer,
                                                    const std::string &username,
                                                    const CertificateStore &certificateStore,
                                                    const DigitalSignature &digitalSignature) {
    sendClientHello(socket, myselfForServer, username);
    receiveServerHello(socket, myselfForServer, certificateStore);
    return endHandshake(socket, myselfForServer, digitalSignature);
}

}
