#include "Tracker.hpp"

Tracker::Tracker(TrackerMaster *trackerMaster) : trackerMaster(trackerMaster) {

}

const std::vector<PeerInfo> &Tracker::getNewPeers() const {
    return peers;
}

void Tracker::addPeer(PeerInfo &peerInfo) {
    peers.push_back(peerInfo);
}


