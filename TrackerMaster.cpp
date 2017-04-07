#include "TrackerMaster.hpp"

TrackerMaster::TrackerMaster(Client *client) : client(client) {

}

void TrackerMaster::addTracker(const std::string &announceUrl) {
    UDPTracker tracker(client, announceUrl);
    trackers.push_back(tracker);
}

void TrackerMaster::getPeers() {
    for (auto &&tracker : trackers) {
        tracker.getPeers();
    }
}
