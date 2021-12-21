#include <iostream>
#include <string>
#include <vector>
#include <Constants.h>
#include <Utils.h>
#include <DigitalSignature.h>
#include <CertificateStore.h>
#include <FourInARow.h>
#include "handler/HandshakeHandler.h"
#include "handler/PreGameHandler.h"
#include "handler/GameHandler.h"

/**
 * Prints a help message describing how to invoke the program from the command line.
 */
void printHelp() {
    std::string helpMessage("Usage: client [-h] -u USERNAME -s ADDRESS -c ADDRESS \n"
                            "\n"
                            "Options:\n"
                            " -h, --help                Show this help message and exit\n"
                            " -u, --username USERNAME   The username of the client\n"
                            " -s, --server   ADDRESS    The IPv4 address of the server\n"
                            " -c, --client   ADDRESS    The IPv4 address of the client");
    std::cout << helpMessage << std::endl;
}

/**
 * Parses the arguments passed via command line. It does not check the validity of
 * the given addresses, which is deferred until the sockets are created,
 * but it tests the validity of the username.
 * The parsing succeeds if all and only the expected arguments are found:
 * if some arguments are missing or unsupported ones are provided,
 * the function fails and automatically prints a help message.
 * @param argc           the number of arguments passed via command line.
 * @param argv           the arguments passed via command line.
 * @param username       a reference to the variable that will store the username.
 * @param serverAddress  a reference to the variable that will store the server address.
 * @param clientAddress  a reference to the variable that will store the client address.
 * @return               true if all and only the required arguments are supplied via
 *                       command line, false otherwise.
 */
bool parseArguments(int argc, char *argv[], std::string &username, std::string &serverAddress, std::string &clientAddress) {
    if (argc != 7) {
        printHelp();
        return false;
    }

    auto optionsFound = 0u;
    for (auto i = 0; i < argc - 1; i++) {
        std::string arg(argv[i]);

        if (arg == "-u" || arg == "--username") {
            username = argv[i + 1];
            optionsFound++;
            i++;
        } else if (arg == "-s" || arg == "--server") {
            serverAddress = argv[i + 1];
            optionsFound++;
            i++;
        } else if (arg == "-c" || arg == "--client") {
            clientAddress = argv[i + 1];
            optionsFound++;
            i++;
        }
    }

    if (optionsFound != 3) {
        printHelp();
        return false;
    }

    try {
        fourinarow::checkUsernameValidity<std::runtime_error>(username);
        return true;
    } catch (const std::runtime_error &exception) {
        std::cerr << exception.what() << std::endl;
        return false;
    }
}

/**
 * Creates a digital signature object using a private key stored in PEM format in a file.
 * @param path  the path of the private key file.
 * @return      the digital signature object.
 * @throws runtime_error  if an error occurs while loading the private key.
 */
fourinarow::DigitalSignature createDigitalSignature(const std::string &path) {
    std::cout << "Creating the digital signature tool using the private key " << path << std::endl;

    try {
        return fourinarow::DigitalSignature(path);
    } catch (const std::exception &exception) {
        std::cerr << "Impossible to create the digital signature tool. " << exception.what() << std::endl;
        throw std::runtime_error("Cannot create the digital signature tool");
    }
}

/**
 * Creates a certificate store loading the certificate of a trusted certification authority and
 * a certificate revocation list, both parsed from file.
 * @param pathCertificate  the path of the file storing the certificate.
 * @param pathCRL          the path of the file storing the certificate revocation list.
 * @return                 the certificate store.
 * @throws runtime_error  if an error occurs while creating the store or loading the items.
 */
fourinarow::CertificateStore createCertificateStore(const std::string &pathCertificate, const std::string &pathCRL) {
    std::cout << "Creating a certificate store holding the CA certificate " << pathCertificate;
    std::cout << " and the CRL " << pathCRL << std::endl;

    try {
        fourinarow::CertificateStore certificateStore;
        certificateStore.addCertificate(pathCertificate);
        certificateStore.addCertificateRevocationList(pathCRL);
        return certificateStore;
    } catch (const std::exception &exception) {
        std::cerr << "Impossible to create the certificate store. " << exception.what() << std::endl;
        throw std::runtime_error("Cannot create the certificate store");
    }
}

/**
 * Creates a socket bound to a given address and connects to a remote server address.
 * @param clientAddress  the address to which the socket will bind.
 * @param serverAddress  the address to which the socket will connect.
 * @return               the new socket.
 * @throws runtime_error  if an error occurs while creating, binding or connecting the socket.
 */
fourinarow::TcpSocket connectToRemoteServer(const std::string &clientAddress, const std::string &serverAddress) {
    std::cout << "Connecting to the remote server " << serverAddress << ':' << fourinarow::SERVER_PORT;
    std::cout << " binding to the address " << clientAddress << ':' << fourinarow::SERVER_PORT << std::endl;

    try {
        fourinarow::TcpSocket serverSocket;

        /*
         * The bind is necessary to let the server know exactly which IP address
         * the client will use for P2P communications. In principle, binding to a specific port
         * is not necessary: to avoid occupying the port reserved for P2P communications
         * by selecting a random number, the port is chosen to be the same one that is
         * used by the server to listen for incoming requests.
         */
        serverSocket.bind(clientAddress, fourinarow::SERVER_PORT);

        serverSocket.connect(serverAddress, fourinarow::SERVER_PORT);
        return serverSocket;
    } catch (const std::exception &exception) {
        std::cerr << "Impossible to connect to the remote server. " << exception.what() << std::endl;
        throw std::runtime_error("Cannot connect to the server");
    }
}

int main(int argc, char *argv[]) {
    try {
        std::string username;
        std::string serverAddress;
        std::string clientAddress;

        if (!parseArguments(argc, argv, username, serverAddress, clientAddress)) {
            return 1;
        }

        auto digitalSignature = createDigitalSignature(fourinarow::CLIENT_KEYS_FOLDER + username + fourinarow::CLIENT_PRIVATE_KEY_SUFFIX);
        auto certificateStore = createCertificateStore(fourinarow::CLIENT_CERTIFICATES_FOLDER + "UnipiCA_cert.pem",
                                                       fourinarow::CLIENT_CERTIFICATES_FOLDER + "UnipiCA_crl.pem");
        auto serverSocket = connectToRemoteServer(clientAddress, serverAddress);

        // Object representing this client when communicating with the server.
        fourinarow::Player myselfForServer;

        // Perform the handshake.
        auto firstPlayerList = fourinarow::HandshakeHandler::doHandshakeWithServer(serverSocket,
                                                                                   myselfForServer,
                                                                                   username,
                                                                                   certificateStore,
                                                                                   digitalSignature);
        while (true) {
            // Handle the pre-game phase.
            fourinarow::PlayerMessage opponent;
            std::string opponentUsername;
            auto playGame = fourinarow::PreGameHandler::handle(serverSocket,myselfForServer, firstPlayerList,opponent,opponentUsername);

            if (!playGame) {
                std::cout << "Goodbye!" << std::endl;
                return 0;
            }

            auto handshakeResult = fourinarow::HandshakeHandler::doHandshakeWithPlayer(clientAddress, opponent, digitalSignature);
            if (std::get<2>(handshakeResult)) {
                fourinarow::GameHandler::handle(*(std::get<0>(handshakeResult)),
                                                *(std::get<1>(handshakeResult)),
                                                opponentUsername,
                                                opponent.isFirstToPlay());
            }
            fourinarow::GameHandler::sendEndGame(serverSocket, myselfForServer);
            firstPlayerList = "";
        }
    } catch (const std::exception &exception) {
        std::cerr << "Fatal error. " << exception.what() << std::endl;
        return 1;
    }
}
