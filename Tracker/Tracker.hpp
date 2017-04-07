#ifndef TORRENT_TRACKER_HPP
#define TORRENT_TRACKER_HPP

#include <string>
#include <vector>

const std::vector<std::string> ACCEPTED_EVENTS = {"", "started", "completed", "stopped"};

typedef struct PeerInfo {
    int32_t ip;
    uint16_t port;
} PeerInfo;


class Tracker {
protected:
    std::vector<PeerInfo> peers;
public:
    virtual void updatePeers() = 0;

    virtual const std::vector<PeerInfo> &getPeers() const;
};


#endif //TORRENT_TRACKER_HPP
