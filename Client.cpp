#include "Client.hpp"


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


void Client::getNewPeers() {
    trackerMaster.getPeers();
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

void Client::addPeer(PeerInfo &peer) {
    peers.push_back(peer);
}






