#include <iostream>
#include <unistd.h>
#include "TrackerMaster.hpp"
#include "../Client.hpp"
#include "HTTPTracker.hpp"
#include "UDPTracker.hpp"
#include "../Utils/Tools.hpp"

TrackerMaster::TrackerMaster(Client *client) : client(client) {

}

void TrackerMaster::addTracker(const std::string &announceUrl) {
    Tracker *tracker = nullptr;
    if (startsWith(announceUrl, "http://")) {
        tracker = new HTTPTracker(this, announceUrl);
    } else if (startsWith(announceUrl, "udp://")) {
        tracker = new UDPTracker(this, announceUrl);
    } else {
        std::cout << "Invalid url " << announceUrl << std::endl;
        return;
    }
    trackers.push_back(tracker);
}

void TrackerMaster::fetchNewPeersFromTracker() {
    for (auto &&tracker : trackers) {
        tracker->updatePeers();

        // TODO: temporary, while this is not multithreaded
        break;
    }

    std::cout << std::endl << std::endl;
    for (const Tracker *tracker : trackers) {
        for (const PeerInfo &peer : tracker->getPeers()) {
            client->handleNewPeer(peer);
            allPeers.insert(peer);
//            std::cout << "Current peer: " << getHumanReadableIP((uint32_t) peer.ip) << std::endl;
        }
    }

}

const int8_t *TrackerMaster::getInfoHash() {
    return client->getInfoHash();
}

const int8_t *TrackerMaster::getPeerID() {
    return client->getPeerID();
}

int64_t TrackerMaster::getNumBytesDownloaded() {
    return client->getNumBytesDownloaded();
}

int64_t TrackerMaster::getNumBytesLeft() {
    return client->getNumBytesLeft();
}

int64_t TrackerMaster::getNumBytesUploaded() {
    return client->getNumBytesUploaded();
}

const std::set<PeerInfo> &TrackerMaster::getAllPeers() const {
    return allPeers;
}

void TrackerMaster::startFetchingPeers() {
    // This should run on a separate thread
    while (true) {
        // TODO: put an end to this madness! aka write while stop condition
        fetchNewPeersFromTracker();
        sleep(10);
    }
}

