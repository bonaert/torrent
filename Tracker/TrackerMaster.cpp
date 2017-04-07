#include "TrackerMaster.hpp"
#include "../Client.hpp"
#include "HTTPTracker.hpp"
#include "UDPTracker.hpp"

TrackerMaster::TrackerMaster(Client *client) : client(client) {

}

void TrackerMaster::addTracker(const std::string &announceUrl) {
    Tracker *tracker = nullptr;
    if (false /* TODO */ ) {
        tracker = new HTTPTracker(this, announceUrl);
    } else {
        tracker = new UDPTracker(this, announceUrl);
    }
    trackers.push_back(tracker);
}

void TrackerMaster::getPeers() {
    for (auto &&tracker : trackers) {
        tracker->updatePeers();
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

