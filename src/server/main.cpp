#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>
#include <string.h>
#include <arpa/inet.h>
#include <Constants.h>
#include <Utils.h>
#include <TcpSocket.h>
#include <TcpSocketHasher.h>
#include <Player.h>
#include <CertificateStore.h>
#include <DigitalSignature.h>
#include <InputMultiplexer.h>
#include "handler/NewClientHandler.h"
#include "handler/ConnectedClientHandler.h"
#include "handler/HandshakeClientHandler.h"
#include "handler/AvailableClientHandler.h"
#include "handler/MatchmakingClientHandler.h"
#include "handler/PlayingClientHandler.h"

using PlayerList = std::unordered_map<fourinarow::TcpSocket, fourinarow::Player, fourinarow::TcpSocketHasher>;
using PlayerStatusList = std::unordered_map<std::string, fourinarow::Player::Status>;
using PlayerRemovalList = std::unordered_set<std::string>;

/**
 * Prints a help message describing how to invoke the program from the command line.
 */
void printHelp() {
    std::string helpMessage("Usage: server [-h] -a ADDRESS \n"
                            "\n"
                            "Options:\n"
                            " -h, --help              Show this help message and exit\n"
                            " -a, --address ADDRESS   The IPv4 address of the server");
    std::cout << helpMessage << std::endl;
}

/**
 * Parses the arguments passed via command line. It does not check the validity of
 * the given server address, which is deferred until the sockets are created.
 * The parsing succeeds if all and only the expected arguments are found:
 * if some arguments are missing or unsupported ones are provided,
 * the function fails and automatically prints a help message.
 * @param argc           the number of arguments passed via command line.
 * @param argv           the arguments passed via command line.
 * @param serverAddress  a reference to the variable that will store the server address.
 * @return               true if all and only the required arguments are supplied via
 *                       command line, false otherwise.
 */
bool parseArguments(int argc, char *argv[], std::string &serverAddress) {
    if (argc != 3) {
        printHelp();
        return false;
    }

    std::string arg(argv[1]);
    if (arg == "-a" || arg == "--address") {
        serverAddress = argv[2];
        return true;
    }

    printHelp();
    return false;
}

/**
 * Loads the certificate of the server from a PEM file and returns it
 * serialized in binary format, ready to be sent through a socket.
 * @param path  the path of the certificate file.
 * @return      the certificate of the server in binary format.
 * @throws runtime_error  if an error occurs while loading the certificate.
 */
std::vector<unsigned char> loadCertificate(const std::string &path) {
    std::cout << "Loading the server certificate " << path << std::endl;

    try {
        return fourinarow::CertificateStore::serializeCertificate(path);
    } catch (const std::exception &exception) {
        std::cerr << "Impossible to load the certificate. " << exception.what() << std::endl;
        throw std::runtime_error("Cannot load the certificate");
    }
}

/**
 * Creates a digital signature object using a private key stored
 * in PEM format in a file.
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
 * Creates a TCP hello socket, binds it to the given address and
 * sets it in a listening state.
 * @param serverAddress  the address to which the socket will bind.
 * @return               the TCP hello socket.
 * @throws runtime_error  if an error occurs while creating the socket.
 */
fourinarow::TcpSocket createHelloSocket(const std::string &serverAddress) {
    std::cout << "Starting the hello socket on " << serverAddress << ':' << fourinarow::SERVER_PORT << std::endl;

    try {
        fourinarow::TcpSocket helloSocket;
        helloSocket.bind(serverAddress, fourinarow::SERVER_PORT);
        helloSocket.listen(fourinarow::BACKLOG_SIZE);
        return helloSocket;
    } catch (const std::exception &exception) {
        std::cerr << "Impossible to start the socket. " << exception.what() << std::endl;
        throw std::runtime_error("Cannot start the hello socket");
    }
}

/**
 * Prints information about the client that is being handled.
 * @param socket  the socket used to communicate with the client.
 * @param player  the player.
 */
void printHandlingInfo(const fourinarow::TcpSocket &socket, const fourinarow::Player &player) {
    std::cout << "Handling a message from " << socket.getFullDestinationAddress();
    std::cout << ". The client state is " << fourinarow::convertClientStatus(player.getStatus());

    if (!player.getUsername().empty()) {
        std::cout << ". Username: " << player.getUsername();
    }
    std::cout << std::endl;
}

/**
 * Chooses the correct handler to manage a client message.
 * @param socket            the socket ready for a <code>receive()</code>.
 * @param player            the player associated to the socket.
 * @param playerList        the player list.
 * @param statusList        the player status list.
 * @param removalList       the player removal list.
 * @param certificate       the certificate of the server.
 * @param digitalSignature  the digital signature tool.
 */
void handleMessage(const fourinarow::TcpSocket &socket,
                   fourinarow::Player &player,
                   PlayerList &playerList,
                   PlayerStatusList &statusList,
                   PlayerRemovalList &removalList,
                   const std::vector<unsigned char> &certificate,
                   const fourinarow::DigitalSignature &digitalSignature) {
    printHandlingInfo(socket, player);

    if (player.getStatus() == fourinarow::Player::Status::CONNECTED) {
        fourinarow::ConnectedClientHandler::handle(socket, player, statusList, removalList, certificate, digitalSignature);
        return;
    }

    if (player.getStatus() == fourinarow::Player::Status::HANDSHAKE) {
        fourinarow::HandshakeClientHandler::handle(socket, player, statusList, removalList);
        return;
    }

    if (player.getStatus() == fourinarow::Player::Status::AVAILABLE) {
        fourinarow::AvailableClientHandler::handle(socket, player, playerList, statusList, removalList);
        return;
    }

    if (player.getStatus() == fourinarow::Player::Status::MATCHMAKING) {
        fourinarow::MatchmakingClientHandler::handle(socket, player, playerList, statusList, removalList);
        return;
    }

    if (player.getStatus() == fourinarow::Player::Status::MATCHMAKING_INTERRUPTED) {
        player.setStatus(fourinarow::Player::Status::AVAILABLE);
        statusList[player.getUsername()] = fourinarow::Player::Status::AVAILABLE;
        std::cout << "Client unblocked: now it is AVAILABLE" << std::endl;
        fourinarow::AvailableClientHandler::handle(socket, player, playerList, statusList, removalList);
        return;
    }

    if (player.getStatus() == fourinarow::Player::Status::PLAYING) {
        fourinarow::PlayingClientHandler::handle(socket, player, statusList, removalList);
        return;
    }

    std::cerr << "Critical error: missing handler. Closing the connection with the client" << std::endl;
    removalList.insert(player.getUsername());
}

/**
 * Checks if the given player is inside the removal list.
 * @param removalList  the removal list.
 * @param player       the player.
 * @return             true if the player is inside the list, false otherwise.
 */
bool isInsideRemovalList(const PlayerRemovalList &removalList, const fourinarow::Player &player) {
    return removalList.count(player.getUsername()) != 0;
}

/**
 * Disconnects the client, removing the corresponding entries in
 * the player list, the player status list and the player removal list.
 * Moreover, the corresponding socket is removed from the multiplexer.
 * The iterator passed to the function is automatically updated to point
 * to the next entry in the player list.
 * @param iterator     the iterator of the player list referring to the client.
 * @param playerList   the player list.
 * @param statusList   the player status list.
 * @param removalList  the player removal list.
 * @param multiplexer  the multiplexer of sockets.
 */
void disconnectClient(PlayerList::iterator &iterator,
                      PlayerList &playerList,
                      PlayerStatusList &statusList,
                      PlayerRemovalList &removalList,
                      fourinarow::InputMultiplexer &multiplexer) {
    removalList.erase(iterator->second.getUsername());
    statusList.erase(iterator->second.getUsername());
    multiplexer.removeDescriptor(iterator->first.getDescriptor());
    iterator = playerList.erase(iterator);
}

/**
 * Prints the current player list.
 * @param playerList  the player list.
 */
void printPlayerList(const PlayerList &playerList) {
    std::string formattedList = "Player list: {";

    if (playerList.empty()) {
        formattedList += "}";
        std::cout << formattedList << std::endl;
        return;
    }

    for (auto &i : playerList) {
        formattedList += (i.second.getUsername()  + ": " + fourinarow::convertClientStatus(i.second.getStatus()) + ", ");
    }

    formattedList.pop_back();
    formattedList.pop_back();
    formattedList += "}";
    std::cout << formattedList << std::endl;
}

/**
 * Prints the current status list.
 * @param statusList  the status list.
 */
void printStatusList(const PlayerStatusList &statusList) {
    std::string formattedList = "Status list: {";

    if (statusList.empty()) {
        formattedList += "}";
        std::cout << formattedList << std::endl;
        return;
    }

    for (auto &i : statusList) {
        formattedList += (i.first + ": " + fourinarow::convertClientStatus(i.second) + ", ");
    }

    formattedList.pop_back();
    formattedList.pop_back();
    formattedList += "}";
    std::cout << formattedList << std::endl;
}

/**
 * Starts the main service loop of the server.
 * @param helloSocket       the hello socket.
 * @param multiplexer       the multiplexer of sockets.
 * @param playerList        the player list.
 * @param statusList        the player status list.
 * @param removalList       the player removal list.
 * @param certificate       the certificate of the server.
 * @param digitalSignature  the digital signature tool.
 */
void startService(fourinarow::TcpSocket &helloSocket,
                  fourinarow::InputMultiplexer &multiplexer,
                  PlayerList &playerList,
                  PlayerStatusList &statusList,
                  PlayerRemovalList &removalList,
                  const std::vector<unsigned char> &certificate,
                  const fourinarow::DigitalSignature &digitalSignature) {
    std::cout << "Initialization performed correctly. Starting the service" << std::endl;

    while (true) {
        std::cout << "Waiting for requests..." << std::endl;
        multiplexer.select();

        // Handle messages from connected clients.
        for (auto iterator = playerList.begin(); iterator != playerList.end();) {
            if (isInsideRemovalList(removalList, iterator->second)) {
                disconnectClient(iterator, playerList, statusList, removalList, multiplexer);
                continue;
            }
            if (multiplexer.isReady(iterator->first.getDescriptor())) {
                handleMessage(iterator->first, iterator->second, playerList, statusList, removalList, certificate, digitalSignature);
                if (isInsideRemovalList(removalList, iterator->second)) {
                    disconnectClient(iterator, playerList, statusList, removalList, multiplexer);
                    continue;
                }
            }
            iterator++;
        }

        // Remove clients that were not removed in the previous loop, if any.
        for (auto iterator = playerList.begin(); iterator != playerList.end();) {
            if (removalList.empty()) {
                break;
            }
            if (isInsideRemovalList(removalList, iterator->second)) {
                disconnectClient(iterator, playerList, statusList, removalList, multiplexer);
                continue;
            }
            iterator++;
        }

        // Handle new connections on the hello socket.
        if (multiplexer.isReady(helloSocket.getDescriptor())) {
            fourinarow::NewClientHandler::handle(helloSocket, multiplexer, playerList);
        }

        printPlayerList(playerList);
        printStatusList(statusList);
    }
}

int main(int argc, char *argv[]) {
    try {
        std::string serverAddress;

        if (!parseArguments(argc, argv, serverAddress)) {
            return 1;
        }

        PlayerList playerList;
        PlayerStatusList statusList; // Fast lookup of player's status.
        PlayerRemovalList removalList;

        auto certificate = loadCertificate(fourinarow::SERVER_CERTIFICATE_FOLDER + "4InARow_cert.pem");
        auto digitalSignature = createDigitalSignature(fourinarow::SERVER_CERTIFICATE_FOLDER + "4InARow_privkey.pem");

        auto helloSocket = createHelloSocket(serverAddress);
        fourinarow::InputMultiplexer multiplexer;
        multiplexer.addDescriptor(helloSocket.getDescriptor());

        startService(helloSocket, multiplexer, playerList, statusList, removalList, certificate, digitalSignature);
    } catch (const std::exception &exception) {
        std::cerr << "Fatal error. " << exception.what() << std::endl;
        return 1;
    }
}
