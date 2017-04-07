#ifndef TORRENT_TRACKERMASTER_HPP
#define TORRENT_TRACKERMASTER_HPP


#include "Tracker.hpp"

class Client;

class UDPTracker;


class TrackerMaster {
private:
    std::vector<UDPTracker> trackers;
    Client *client;
public:
    TrackerMaster(Client *client);

    void addTracker(const std::string &announceUrl);

    void getPeers();
};


#endif //TORRENT_TRACKERMASTER_HPP
