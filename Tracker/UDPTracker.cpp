#include <assert.h>
#include <sstream>
#include <iostream>
#include "UDPTracker.hpp"
#include "../Utils/Tools.hpp"


/*
 *
 * UDP TRACKER
 *
 * http://www.bittorrent.org/beps/bep_0015.html
 *
 */

UDPTracker::UDPTracker(TrackerMaster *trackerMaster, const std::string &announceURL) :
        Tracker(trackerMaster),
        connectionID(-1),
        transactionID(-1) {
    trackerDomain = getDomainFromUrl(announceURL);
    int trackerPort = getPortFromUrl(announceURL);
    udpCommunicator = new UDPCommunicator(trackerDomain, trackerPort, DEFAULT_PORT);
}

void UDPTracker::updatePeers() {
    peers.clear();
    bool foundServer = udpCommunicator->connect();
    if (foundServer) {
        std::cout << "Connected to tracker " << trackerDomain << std::endl;
        bool success = connectToServer();
        if (success) {
            getPeersFromServer();
        }
    }
    udpCommunicator->disconnect();
}

bool UDPTracker::getPeersFromServer() {
    bool success = sendGetPeersRequest();
    if (!success) {
        return false;
    }

    return processGetPeersResponse();
}

bool UDPTracker::sendGetPeersRequest() {
    AnnounceRequest request = buildAnnounceRequest();
    char *buffer = nullptr;
    int length;
    writeAnnounceRequestToBuffer(request, &buffer, &length); // Creates and fills buffer, updates length

    int bytesSend = udpCommunicator->sendRequest(buffer, length);
    if (bytesSend < length) {
        perror("UDP Announce send failed");
        return false;
    } else {
        return true;
    }
}

bool UDPTracker::processGetPeersResponse() {
    int BUFFER_SIZE = 4000;
    char response[BUFFER_SIZE];
    int bytesRead = udpCommunicator->receiveResponse(response, BUFFER_SIZE);

    if (bytesRead < 20) {
        std::cout << "Didn't receive enough bytes for a complete response" << std::endl;
        return false;
    }

    AnnounceResponse announceResponse(response, bytesRead);
    if (announceResponse.transactionID != transactionID) {
        std::cout << "Response is a different transaction. Abort" << std::endl;
        return false;
    }

    for (PeerInfo &peer : announceResponse.peers) {
        addPeer(peer);
//        std::cout << "Added leecher with IP " << getHumanReadableIP((uint32_t) peer.ip) << std::endl;
    }

    std::cout << "Found " << announceResponse.peers.size() << " new peers." << std::endl;
    return true;
}


AnnounceRequest UDPTracker::buildAnnounceRequest() const {
    AnnounceRequest request;
    request.connectionID = connectionID;
    request.transactionID = transactionID;

    const int8_t *infoHash = trackerMaster->getInfoHash();
    copyArray((char *) infoHash, (char *) request.infoHash, 20);

    const int8_t *peerID = trackerMaster->getPeerID();
    copyArray((char *) peerID, (char *) request.peerID, 20);

    request.numBytesDownloaded = trackerMaster->getNumBytesDownloaded();
    request.numBytesUploaded = trackerMaster->getNumBytesUploaded();
    request.numBytesLeft = trackerMaster->getNumBytesLeft();
    request.event = 0; // Started. TODO: change this later as appropriate
    request.port = (uint16_t) udpCommunicator->getSourcePort();

    return request;
}

bool UDPTracker::connectToServer() {
    bool success = sendConnectRequest();
    if (!success) {
        return false;
    }

    return processConnectResponse();
}


bool UDPTracker::sendConnectRequest() {
    char *buffer = nullptr;
    int length;
    ConnectRequest request = ConnectRequest();
    transactionID = request.transactionID;
    writeConnectRequestToBuffer(request, &buffer, &length); // Creates and fills buffer, updates length

    int bytesSend = udpCommunicator->sendRequest(buffer, length);
    if (bytesSend < length) {
        perror("UDP Handshake send failed");
        return false;
    } else {
        return true;
    }
}

bool UDPTracker::processConnectResponse() {
    char response[16];
    int bytesRead = udpCommunicator->receiveResponse(response, 16);
    if (0 < bytesRead && bytesRead < 16) {
        std::cout << "UDP Handshake: Read only " << bytesRead << "of the 16 wanted bytes" << std::endl;
    }

    if (bytesRead != 16) {
        return false;
    }

    ConnectResponse connectResponse(response);
    if (connectResponse.transactionID != transactionID) {
        std::cout << "Response is a different transaction. Abort" << std::endl;
        return false;
    }

    connectionID = connectResponse.connectionID;
    return true;
}


void UDPTracker::writeAnnounceRequestToBuffer(AnnounceRequest &request, char **bufferPointer, int *length) {
    *length = 496;
    *bufferPointer = new char[496];
    char *buffer = *bufferPointer;

    buffer = writeInt64(buffer, request.connectionID);
    buffer = writeInt32(buffer, request.action);
    buffer = writeInt32(buffer, request.transactionID);
    buffer = writeInt8Array(buffer, request.infoHash, 20);
    buffer = writeInt8Array(buffer, request.peerID, 20);
    buffer = writeInt64(buffer, request.numBytesDownloaded);
    buffer = writeInt64(buffer, request.numBytesLeft);
    buffer = writeInt64(buffer, request.numBytesUploaded);
    buffer = writeInt32(buffer, request.event);
    buffer = writeInt32(buffer, request.IPAdress);
    buffer = writeInt32(buffer, request.key);
    buffer = writeInt32(buffer, request.numPeersWanted);
    buffer = writeInt16(buffer, request.port);
    buffer = writeInt16(buffer, request.extensions);
}


void writeConnectRequestToBuffer(const ConnectRequest &request, char **bufferPointer, int *length) {
    *length = 4 * 4;
    *bufferPointer = (char *) new int32_t[4];
    char *buffer = *bufferPointer;

    buffer = writeInt64(buffer, request.protocolID);
    buffer = writeInt32(buffer, request.action);
    buffer = writeInt32(buffer, request.transactionID);
}


ConnectResponse::ConnectResponse(char response[16]) {
    action = readInt32AndAdvancePointer(&response);
    assert(action == 0);
    transactionID = readInt32AndAdvancePointer(&response);
    connectionID = readInt64AndAdvancePointer(&response);
}

AnnounceResponse::AnnounceResponse(char *buffer, int numChars) {
    action = readInt32AndAdvancePointer(&buffer);
    assert(action == 1 or action == 3);

    if (action == 3) { // Error
        // TODO: handle this
        return;
    }

    transactionID = readInt32AndAdvancePointer(&buffer);
    intervalBetweenAnnouceRequests = readInt32AndAdvancePointer(&buffer);
    numLeechers = readInt32AndAdvancePointer(&buffer);
    numSeeders = readInt32AndAdvancePointer(&buffer);

    const int numPeers = (numChars - 20) / 6;
    for (int i = 0; i < numPeers; ++i) {
        PeerInfo peer;
        peer.ip = readInt32AndAdvancePointer(&buffer);
        peer.port = (uint16_t) readInt16AndAdvancePointer(&buffer);
        peers.push_back(peer);
    }

}