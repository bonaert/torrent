#include "UDPCommunicator.hpp"

UDPCommunicator::UDPCommunicator(const std::string &hostName, int hostPort, int sourcePort) :
        sourcePort(sourcePort) {
    setServer(hostName, hostPort);
}

bool UDPCommunicator::setServer(const std::string &name, int port) {
    hostName = name;
    hostPort = port;
    socket = createUDPSocket(sourcePort);
    serverAddress = buildAddress(name, port);
    return (serverAddress != nullptr);
}

int UDPCommunicator::sendRequest(char *buffer, int size) {
    ssize_t errorCode = sendto(socket, buffer, (size_t) size, 0, (const sockaddr *) serverAddress, sizeof(sockaddr));
    if (errorCode == -1) {
        perror("Couldn't send UDP request");
    }
    return (int) errorCode;
}

void UDPCommunicator::sendRequest(const std::string &message) {
    ssize_t errorCode = sendto(socket, message.c_str(), message.size(), 0, (const sockaddr *) serverAddress,
                               sizeof(sockaddr));
    if (errorCode == -1) {
        perror("Couldn't send UDP request");
    }
}

std::string UDPCommunicator::receiveResponse(int size) {
    char buffer[size];
    socklen_t length;
    ssize_t bytesRead = recvfrom(socket, buffer, (size_t) size, 0, (sockaddr *) serverAddress, &length);

    if (bytesRead == -1) {
        perror("Couldn't receive UDP datagram");
    }

    return std::string(buffer, (unsigned long) bytesRead);
}

int UDPCommunicator::receiveResponse(char *buffer, int size) {
    socklen_t length = sizeof(serverAddress);
    ssize_t bytesRead = recvfrom(socket, buffer, (size_t) size, 0, (sockaddr *) serverAddress, &length);

    if (bytesRead == -1) {
        perror("Couldn't receive UDP datagram");
    }
    return (int) bytesRead;
}

bool UDPCommunicator::foundServer() {
    return serverAddress != nullptr;
}

int UDPCommunicator::getSourcePort() const {
    return sourcePort;
}

