#include "Client.hpp"
#include "Peer/PeerManager.hpp"


Client::Client(std::string &filename) :
        trackerMaster(nullptr),
        peerManager(nullptr),
        hasStarted(false),
        torrent(filename) {
    setup();
}

Client::Client(Torrent &torrent) :
        trackerMaster(nullptr),
        peerManager(nullptr),
        hasStarted(false),
        torrent(torrent) {
    setup();
}

void Client::setup() {
    buildPeerID();
    numBytesUploaded = 0;
    numBytesDownloaded = 0;
    numBytesLeft = torrent.size();

    trackerMaster = new TrackerMaster(this);
    trackerMaster->addTracker(torrent.metaInfo.announceURL);
    for (auto &&announceUrl : torrent.metaInfo.announceList) {
        trackerMaster->addTracker(announceUrl);
    }
}


void Client::start() {
    if (hasStarted) {
        throw std::logic_error("Client has already started!");
    }

    peerManager = new PeerManager(this); // Starts the peer thread pool automatically
    trackerMaster->startFetchingPeers();
    hasStarted = true;
}

void Client::handleNewPeer(PeerInfo peer) {
    peerManager->handleNewPeerFound(peer);
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












