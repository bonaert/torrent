#ifndef TORRENT_TRACKER_HPP
#define TORRENT_TRACKER_HPP

#include <string>
#include <vector>
#include "TrackerMaster.hpp"


class TrackerMaster;
const std::vector<std::string> ACCEPTED_EVENTS = {"", "started", "completed", "stopped"};
static const int DEFAULT_PORT = 6881;

typedef struct PeerInfo {
    int32_t ip;
    uint16_t port;
} PeerInfo;



class Tracker {
protected:
    std::vector<PeerInfo> peers;
    TrackerMaster *trackerMaster;
public:
    Tracker(TrackerMaster *trackerMaster);

    virtual void updatePeers() = 0;

    virtual const std::vector<PeerInfo> &getPeers() const;

    virtual void addPeer(PeerInfo &peerInfo);
};


#endif //TORRENT_TRACKER_HPP
