#ifndef TORRENT_TRACKERMASTER_HPP
#define TORRENT_TRACKERMASTER_HPP


#include "Tracker.hpp"


class Client;

class Tracker;


class TrackerMaster {
private:
    std::vector<Tracker *> trackers;
    Client *client;
public:
    TrackerMaster(Client *client);

    void addTracker(const std::string &announceUrl);

    void getPeers();

    const int8_t *getInfoHash();

    const int8_t *getPeerID();

    int64_t getNumBytesDownloaded();

    int64_t getNumBytesUploaded();

    int64_t getNumBytesLeft();
};


#endif //TORRENT_TRACKERMASTER_HPP
