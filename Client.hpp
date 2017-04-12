#ifndef TORRENT_CLIENT_HPP
#define TORRENT_CLIENT_HPP

#include <string>
#include "Torrent.hpp"
#include "Tracker/Tracker.hpp"


class TrackerMaster;
class PeerManager;

class Client {
    Torrent torrent;
    TrackerMaster *trackerMaster;
    PeerManager *peerManager;
    int8_t peerID[20];
    bool hasStarted;

    /* Tracker information */
    int numBytesUploaded;
    int numBytesDownloaded;
    int numBytesLeft;

    void buildPeerID();

public:
    Client(std::string &filename);

    Client(Torrent &torrent);

    void setup();


    const int8_t *getPeerID() const;

    const int8_t *getInfoHash() const;

    int getNumBytesUploaded() const;

    int getNumBytesDownloaded() const;

    int getNumBytesLeft() const;

    void start();

    void handleNewPeer(PeerInfo peer);
};


#endif //TORRENT_CLIENT_HPP
