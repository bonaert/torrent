#ifndef TORRENT_UDPCOMMUNICATOR_HPP
#define TORRENT_UDPCOMMUNICATOR_HPP


#include <string>
#include "Networking.hpp"

class UDPCommunicator {
private:
    int socket = -1;

    std::string hostName;
    int hostPort;

    int sourcePort;
public:
    int getSourcePort() const;

private:

    sockaddr_in *serverAddress;

public:
    UDPCommunicator(int sourcePort);

    UDPCommunicator(const std::string &hostName, int hostPort, int sourcePort);

    bool setServer(const std::string &name, int port);

    void sendRequest(const std::string &message);

    int sendRequest(char *buffer, int size);

    bool foundServer();

    std::string receiveResponse(int size);

    int receiveResponse(char *buffer, int size);

    bool connect();

    bool disconnect();
};


#endif //TORRENT_UDPCOMMUNICATOR_HPP
