#include <iostream>
#include "Client.hpp"
#include <curl/curl.h>


Client::Client(std::string &filename) : trackerMaster(this), torrent(filename) {
    setup();
}

Client::Client(Torrent &torrent) : trackerMaster(this), torrent(torrent) {
    setup();
}

void Client::setup() {
    buildPeerID();
    numBytesUploaded = 0;
    numBytesDownloaded = 0;
    numBytesLeft = torrent.size();

    trackerMaster.addTracker(torrent.metaInfo.announceURL);
    for (auto &&announceUrl : torrent.metaInfo.announceList) {
        trackerMaster.addTracker(announceUrl);
    }
}


/*
 * Here are a list of urls I found in a couple of torrent files:
 *   - "udp://tracker.istole.it:80/announce"
 *   - "udp://tracker.1337x.org:80"
 *   - "udp://fr33dom.h33t.com:3310/announce"
 */


void Client::sendGetPeersUDPRequestToTracker() {
    trackerMaster.getPeers();
//    UDPTracker tracker(this, torrent.metaInfo.announceURL);
//    tracker.getPeers();
    /*
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
     */
}

bool Client::sendGetPeersUDPRequestToTracker(const std::string &announceURL) {

}


/*
 * LEGACY
 */

void Client::sendGetPeersRequestToTracker() {
    sendGetPeersRequestToTracker("");
}

void Client::sendGetPeersRequestToTracker(const std::string &event) {
    for (std::string &annouceUrl : torrent.metaInfo.announceList) {
        sendGetPeersRequestToTracker(annouceUrl, event);
    }
}

void Client::sendGetPeersRequestToTracker(const std::string &announce, const std::string &event) {
//    std::string protocolLessURL = announce.substr(6, announce.size() - 6);
/*
    TrackerRequest request(announce);
    std::string requestMessage = request
            .addInfoHash(torrent.metaInfo.infoDictHash)
            .addPeerID(peerID)
            .addPort(-1) // BUG
            .addUploadedBytes(numBytesUploaded)
            .addDownloadedBytes(numBytesDownloaded)
            .addBytesLeft(numBytesLeft)
            .addAcceptsCompactResponseFlag(false)
            .addEvent(event)
            .getRequestURL();

    sendRequest(requestMessage);
    */
}


void Client::processGetPeersResponseFromTracker(const std::string &response) {
    /*
    if (latestTrackerResponse == nullptr) {
        latestTrackerResponse = buildTrackerResponse(response);
    } else {
        *latestTrackerResponse = *buildTrackerResponse(response);
    }


    // Handle error cases
    if (latestTrackerResponse->failed) {
        std::cout << "The tracker couldn't send peers for the following reason:" << std::endl;
        std::cout << latestTrackerResponse->failureReason << std::endl;
        return;
    }

    if (latestTrackerResponse->warning.length() != 0) {
        std::cout << "The tracker sent the following warning:" << std::endl;
        std::cout << latestTrackerResponse->warning << std::endl;
    }



    // Process response
    if (latestTrackerResponse->trackerID.size() != 0) {
        trackerID = latestTrackerResponse->trackerID; // Update the tracker ID
    }

    updatePeers();
    */
}



void Client::updatePeers() {
    // TODO: for now, it only appends the new peers
    // However, there could be duplicates, which is bad, so later I'll have to fix
    // This could be done easily by using a set, and implementing a comparison
    // operation on the peers that only compares their peer ID
/*
    for (const Peer &peer : latestTrackerResponse->peers) {
        peers.push_back(peer);
    }
    */
}


/*
 * UTILITIES
 */


void Client::buildPeerID() {
    std::string ID = "-UT0001-";
    for (int j = 0; j < 8; ++j) {
        peerID[j] = ID[j];
    }

    for (int i = 8; i < 20; ++i) { // Add 12 random bytes
        peerID[i] = (char) rand();
    }
}


/*
 * NETWORKING
 */


void Client::sendRequest(std::string url) {
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_LOCALPORT, 6881);

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);

        /* Check for errors */
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        /* always cleanup */
        curl_easy_cleanup(curl);
    }
}

const int8_t *Client::getPeerID() const {
    return peerID;
}

int Client::getNumBytesUploaded() const {
    return numBytesUploaded;
}

int Client::getNumBytesDownloaded() const {
    return numBytesDownloaded;
}

int Client::getNumBytesLeft() const {
    return numBytesLeft;
}

const int8_t *Client::getInfoHash() const {
    return (const int8_t *) torrent.metaInfo.infoDictHash;
}

void Client::addPeer(PeerInfo &peer) {
    peers.push_back(peer);
}






