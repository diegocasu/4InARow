#include <iostream>
#include <chrono>
#include <thread>
#include <Utils.h>
#include <SerializationException.h>
#include <CryptoException.h>
#include <SocketException.h>
#include <ClientHello.h>
#include <ServerHello.h>
#include <EndHandshake.h>
#include <Player1Hello.h>
#include <Player2Hello.h>
#include <InfoMessage.h>
#include <PlayerListMessage.h>
#include <DigitalSignature.h>
#include <InputMultiplexer.h>
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

void HandshakeHandler::receiveServerHello(const TcpSocket &socket,
                                          Player &myselfForServer,
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

std::string HandshakeHandler::endHandshake(const TcpSocket &socket,
                                           Player &myselfForServer,
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

void HandshakeHandler::connectToPlayer(TcpSocket &socket, const std::string &otherPlayerAddress) {
    auto attempts = 0u;

    while (attempts < P2P_MAX_CONNECTION_RETRIES) {
        try {
            socket.connect(otherPlayerAddress, PLAYER_PORT);
            return;
        } catch (const std::exception &exception) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            attempts++;
        }
    }

    throw SocketException("Max number of connection attempts reached");
}

TcpSocket HandshakeHandler::waitForPlayerConnection(TcpSocket &socket,
                                                    const std::string &otherPlayerAddress) {
    socket.listen(BACKLOG_SIZE);

    InputMultiplexer multiplexer;
    multiplexer.addDescriptor(socket.getDescriptor());
    auto attempts = 0u;

    while (attempts < P2P_MAX_CONNECTION_RETRIES) {
        try {
            multiplexer.selectWithTimeout(1);
            auto playerSocket = socket.accept();

            if (playerSocket.getDestinationAddress() != otherPlayerAddress) {
                throw std::runtime_error("Wrong player connected");
            }

            return playerSocket;
        } catch (const std::exception &exception) {
            attempts++;
        }
    }

    throw SocketException("The other player did not connect");
}

void HandshakeHandler::sendPlayer1Hello(const TcpSocket &socket, Player &myselfForOpponent) {
    std::cout << "Handshake: sending a PLAYER1_HELLO message" << std::endl;

    try {
        myselfForOpponent.generateClientNonce();
        myselfForOpponent.generateClientKeys();

        socket.send(Player1Hello(myselfForOpponent.getClientNonce(),
                                 myselfForOpponent.getClientPublicKey()
                                 ).serialize());
    }  catch (const std::exception &exception) {
        std::cerr << "Impossible to start the handshake: " << exception.what() << std::endl;
        throw std::runtime_error("Handshake with the player failed");
    }
}

void HandshakeHandler::handlePlayer1Hello(const TcpSocket &socket,
                                          Player &opponent,
                                          const DigitalSignature &digitalSignature) {
    std::cout << "Handshake: waiting for a PLAYER1_HELLO message" << std::endl;

    try {
        auto message = socket.receive();
        auto type = getMessageType<SerializationException>(message);

        if (type != PLAYER1_HELLO) {
            socket.send(InfoMessage(PROTOCOL_VIOLATION).serialize());
            // SocketException to avoid sending a MALFORMED_MESSAGE in addition to PROTOCOL_VIOLATION.
            throw SocketException(convertMessageType(type));
        }

        Player1Hello player1Hello;
        player1Hello.deserialize(message);

        opponent.generateServerNonce();
        opponent.generateServerKeys();
        opponent.setClientNonce(player1Hello.getNonce());
        opponent.setClientPublicKey(player1Hello.getPublicKey());
        opponent.generateFreshnessProofP2P();

        std::cout << "Handshake: responding with a PLAYER2_HELLO message" << std::endl;
        socket.send(Player2Hello(opponent.getServerNonce(),
                                 opponent.getServerPublicKey(),
                                 digitalSignature.sign(opponent.getFreshnessProof())
                                 ).serialize());
        return;
    } catch (const SocketException &exception) {
        std::cerr << "Impossible to start the handshake: " << exception.what() << std::endl;

    } catch (const SerializationException &exception) {
        std::cerr << "Impossible to start the handshake: " << exception.what() << std::endl;
        socket.send(InfoMessage(MALFORMED_MESSAGE).serialize());

    } catch (const std::exception &exception) {
        std::cerr << "Impossible to start the handshake: " << exception.what() << std::endl;
        socket.send(InfoMessage(INTERNAL_ERROR).serialize());
    }
    throw std::runtime_error("Handshake with the player failed");
}

void HandshakeHandler::receivePlayer2Hello(const TcpSocket &socket,
                                           Player &myselfForOpponent,
                                           const PlayerMessage &playerMessage) {
    std::cout << "Handshake: waiting for a PLAYER2_HELLO message" << std::endl;

    try {
        auto message = socket.receive();
        auto type = getMessageType<SerializationException>(message);

        if (type != PLAYER2_HELLO) {
            throw SerializationException(convertMessageType(type));
        }

        Player2Hello player2Hello;
        player2Hello.deserialize(message);

        myselfForOpponent.setServerNonce(player2Hello.getNonce());
        myselfForOpponent.setServerPublicKey(player2Hello.getPublicKey());
        myselfForOpponent.generateFreshnessProofP2P();

        if (!DigitalSignature::verify(myselfForOpponent.getFreshnessProof(),
                                      player2Hello.getDigitalSignature(),
                                      playerMessage.getPublicKey())) {
            throw CryptoException("Invalid signature of the freshness proof");
        }
    } catch (const std::exception &exception) {
        std::cerr << "Impossible to continue the handshake: " << exception.what() << std::endl;
        throw std::runtime_error("Handshake with the player failed");
    }
}

void HandshakeHandler::handleEndHandshakeP2P(const TcpSocket &socket,
                                             Player &opponent,
                                             const PlayerMessage &playerMessage) {
    std::cout << "Handshake: waiting for an END_HANDSHAKE message" << std::endl;

    try {
        auto message = socket.receive();
        auto type = getMessageType<SerializationException>(message);

        if (type != END_HANDSHAKE) {
            socket.send(InfoMessage(PROTOCOL_VIOLATION).serialize());
            // SocketException to avoid sending a MALFORMED_MESSAGE in addition to PROTOCOL_VIOLATION.
            throw SocketException(convertMessageType(type));
        }

        EndHandshake endHandshake;
        endHandshake.deserialize(message);

        if (!DigitalSignature::verify(opponent.getFreshnessProof(),
                                      endHandshake.getDigitalSignature(),
                                      playerMessage.getPublicKey())) {
            throw CryptoException("Invalid signature of the freshness proof");
        }

        opponent.initCipher();
        return;
    } catch (const SocketException &exception) {
        std::cerr << "Error while finalizing the handshake: " << exception.what() << std::endl;

    } catch (const SerializationException &exception) {
        std::cerr << "Error while finalizing the handshake: " << exception.what() << std::endl;
        socket.send(InfoMessage(MALFORMED_MESSAGE).serialize());

    } catch (const std::exception &exception) {
        std::cerr << "Error while finalizing the handshake: " << exception.what() << std::endl;
        socket.send(InfoMessage(INTERNAL_ERROR).serialize());
    }
    throw std::runtime_error("Handshake with the player failed");
}

void HandshakeHandler::endHandshakeP2P(const TcpSocket &socket,
                                       Player &myselfForOpponent,
                                       const DigitalSignature &digitalSignature) {
    std::cout << "Handshake: sending an END_HANDSHAKE message" << std::endl;

    try {
        auto signature = digitalSignature.sign(myselfForOpponent.getFreshnessProof());
        EndHandshake endHandshake(signature);
        socket.send(endHandshake.serialize());

        myselfForOpponent.initCipher();
    } catch (const std::exception &exception) {
        std::cerr << "Impossible to finalize the handshake: " << exception.what() << std::endl;
        throw std::runtime_error("Handshake with the player failed");
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

HandshakeHandler::P2PHandshakeResult HandshakeHandler::doHandshakeWithPlayer(const std::string &myAddress,
                                                                             const PlayerMessage &playerMessage,
                                                                             const DigitalSignature &digitalSignature) {
    P2PHandshakeResult result(nullptr, nullptr, false);

    try {
        std::get<0>(result) = std::make_unique<TcpSocket>();
        std::get<0>(result)->bind(myAddress, PLAYER_PORT);
        std::get<1>(result) = std::make_unique<Player>();

        if (playerMessage.isFirstToPlay()) {
            connectToPlayer(*(std::get<0>(result)), playerMessage.getIpAddress());
            sendPlayer1Hello(*(std::get<0>(result)), *(std::get<1>(result)));
            receivePlayer2Hello(*(std::get<0>(result)), *(std::get<1>(result)), playerMessage);
            endHandshakeP2P(*(std::get<0>(result)), *(std::get<1>(result)), digitalSignature);

            std::get<2>(result) = true;
            return result;
        }

        auto playerSocket = waitForPlayerConnection(*(std::get<0>(result)), playerMessage.getIpAddress());
        std::get<0>(result).reset();
        std::get<0>(result) = std::make_unique<TcpSocket>(std::move(playerSocket));

        handlePlayer1Hello(*(std::get<0>(result)), *(std::get<1>(result)), digitalSignature);
        handleEndHandshakeP2P(*(std::get<0>(result)), *(std::get<1>(result)), playerMessage);
        std::get<2>(result) = true;
        return result;
    } catch (const std::exception &exception) {
        std::cout << "Impossible to connect to the other player: " << exception.what() << std::endl;
        return result;
    }
}

}
