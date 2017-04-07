#include <assert.h>
#include <sstream>
#include <iostream>
#include "Tracker.hpp"
#include "Utils/Tools.hpp"


/*
 *
 * TRACKER UDP REQUEST
 *
 * http://www.bittorrent.org/beps/bep_0015.html
 *
 */


/*
 * UDP Tracker
 */

UDPTracker::UDPTracker(Client *client, const std::string &announceURL) : client(client),
                                                                         connectionID(-1),
                                                                         transactionID(-1) {
    const std::string &trackerDomain = getDomainFromUrl(announceURL);
    int trackerPort = getPortFromUrl(announceURL);
    udpCommunicator = new UDPCommunicator(trackerDomain, trackerPort, DEFAULTPORT);
}

void UDPTracker::getPeers() {
    if (udpCommunicator->foundServer()) {
        connectToServer();
        getPeersFromServer();
    }
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
        client->addPeer(peer);
        std::cout << "Added leecher with IP " << getHumanReadableIP((uint32_t) peer.ip) << std::endl;
    }
    return true;
}


AnnounceRequest UDPTracker::buildAnnounceRequest() const {
    AnnounceRequest request;
    request.connectionID = connectionID;
    request.transactionID = transactionID;

    const int8_t *infoHash = client->getInfoHash();
    copyArray((char *) infoHash, (char *) request.infoHash, 20);

    const int8_t *peerID = client->getPeerID();
    copyArray((char *) peerID, (char *) request.peerID, 20);

    request.numBytesDownloaded = client->getNumBytesDownloaded();
    request.numBytesUploaded = client->getNumBytesUploaded();
    request.numBytesLeft = client->getNumBytesLeft();
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


/*
 *
 * TRACKER HTTP REQUEST (LEGACY)
 *
 */


TrackerRequest::TrackerRequest(std::string announceUrl): requestString(announceUrl + '?'),
                                                         addedFirstParameter(false)
{
}

/*
 * UTILITY METHODS TO ADD PARAMETERS TO THE REQUEST STRING
 */

void TrackerRequest::addKeyInRequest(const std::string &key) {
    if (addedFirstParameter) {
        requestString += '&';
    } else {
        addedFirstParameter = true;
    }
    requestString += key;
}

void TrackerRequest::addKeyValuePairInRequest(const std::string &key, const std::string &value) {
    if (addedFirstParameter) {
        requestString += '&';
    } else {
        addedFirstParameter = true;
    }
    requestString += key + '=' + value;
}

void TrackerRequest::addUrlEncodedKeyInRequest(const std::string &key) {
    if (addedFirstParameter) {
        requestString += '&';
    } else {
        addedFirstParameter = true;
    }
    requestString += urlencode(key);
}

void TrackerRequest::addUrlEncodedKeyValuePairInRequest(const std::string &key, const std::string &value) {
    if (addedFirstParameter) {
        requestString += '&';
    } else {
        addedFirstParameter = true;
    }
    requestString += urlencode(key) + '=' + urlencode(value);
}


TrackerRequest &TrackerRequest::addInfoHash(const std::string& infoHash) {
    assert(infoHash.length() == HASH_SIZE);
    addUrlEncodedKeyValuePairInRequest("info_hash", infoHash);
    return *this;
}

TrackerRequest &TrackerRequest::addPeerID(const std::string &peerID) {
    assert(peerID.length() == HASH_SIZE);
    addUrlEncodedKeyValuePairInRequest("peer_id", peerID);
    return *this;
}

TrackerRequest &TrackerRequest::addPort(int port) {
    addKeyValuePairInRequest("port", std::to_string(port));
    return *this;
}

TrackerRequest &TrackerRequest::addUploadedBytes(int uploadedBytes) {
    addKeyValuePairInRequest("uploaded", std::to_string(uploadedBytes));
    return *this;
}

TrackerRequest &TrackerRequest::addDownloadedBytes(int downloadedBytes) {
    addKeyValuePairInRequest("downloaded", std::to_string(downloadedBytes));
    return *this;
}

TrackerRequest &TrackerRequest::addBytesLeft(int bytesLeft) {
    addKeyValuePairInRequest("left", std::to_string(bytesLeft));
    return *this;
}

TrackerRequest &TrackerRequest::addAcceptsCompactResponseFlag(bool compactFlag) {
    if (compactFlag){
        addKeyValuePairInRequest("compact", "1");
    } else {
        addKeyValuePairInRequest("compact", "0");
    }
    return *this;
}

TrackerRequest &TrackerRequest::addNoPeerIDFlag() {
    addKeyInRequest("no_peer_id");
    return *this;
}

TrackerRequest &TrackerRequest::addEvent(const std::string &event) {
    assert(contains(ACCEPTED_EVENTS, event));
    addKeyValuePairInRequest("event", event);
    return *this;
}

TrackerRequest &TrackerRequest::addIP(const std::string &ip) {
    addKeyValuePairInRequest("ip", ip);
    return *this;
}

TrackerRequest &TrackerRequest::addNumberPeersWanted(int numPeersWanted) {
    addKeyValuePairInRequest("numwant", std::to_string(numPeersWanted));
    return *this;
}

TrackerRequest &TrackerRequest::addKey(const std::string &key) {
    addUrlEncodedKeyValuePairInRequest("key", key);
    return *this;
}

TrackerRequest &TrackerRequest::addTrackerID(const std::string &trackerID) {
    addUrlEncodedKeyValuePairInRequest("trackerid", trackerID);
    return *this;
}

const std::string &TrackerRequest::getRequestURL() {
    return requestString;
}


/*
 *
 * TRACKER RESPONSE
 *
 */


TrackerResponse *buildTrackerResponse(const std::string &response) {
    std::istringstream stream(response);

    BDictionary *responseDict = static_cast<BDictionary *>(parseBItem(stream));
    TrackerResponse *trackerResponse = new TrackerResponse(*responseDict);
    delete responseDict;

    return trackerResponse;
}

TrackerResponse::TrackerResponse(const BDictionary &response) {
    parseResponse(response);
}

void TrackerResponse::parseResponse(const BDictionary & response) {
    if (response.contains("failure reason")){
        failed = true;
        failureReason = response.getString("failure reason");
        return;
    } else {
        failed = false;
    }


    if (response.contains("warning message")) {
        warning = response.getString("warning message");
    }


    intervalBetweenRequests = response.getInt("interval");
    if (response.contains("min interval")) {
        minimumIntervalBetweenRequests = response.getInt("min interval");
    }

    if (response.contains("tracker id")) {
        trackerID = response.getString("tracker id");
    }

    numSeeders = response.getInt("complete");
    numLeechers = response.getInt("incomplete");

    /* Parse peers list */
    // TODO: maybe parse binary format, as described in
    // https://wiki.theory.org/BitTorrentSpecification#Tracker_Response
    // Currently we're assuming we'll only receive the dictionary model of peers


    const BList &peersList = response.getList("peers");
    for (int i = 0; i < peersList.size(); ++i) {
        const BDictionary& peerDict = peersList.getDictionary(i);
        Peer peer;
        peer.peerID = peerDict.getString("peer id");
        peer.ip = peerDict.getString("ip");
        peer.port = peerDict.getInt("port");
        peers.push_back(peer);
    }
}






/*
void Client::sendGetPeersUDPRequestToTracker() {
    bool success = sendGetPeersUDPRequestToTracker(torrent.metaInfo.announceURL);
    if (success) {
        return;
    }

    for (const std::string & announceUrl: torrent.metaInfo.announceList) {
        success = sendGetPeersUDPRequestToTracker(announceUrl);
        if (success) {
            return;
        }
    }
}
 */



