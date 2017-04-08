#ifndef TORRENT_CLIENT_HPP
#define TORRENT_CLIENT_HPP

#include <string>
#include "Torrent.hpp"
#include "Tracker/TrackerMaster.hpp"
#include "Peer/PeerConnection.hpp"


class TrackerMaster;

class Client {
    Torrent torrent;
    TrackerMaster trackerMaster;
    int8_t peerID[20];

    /* Tracker information */
    int numBytesUploaded;
    int numBytesDownloaded;
    int numBytesLeft;


    std::set<PeerConnection> peerConnections;


    void buildPeerID();

public:
    Client(std::string &filename);

    Client(Torrent &torrent);

    void setup();

    /* Trackers communication */
    void getNewPeers();

    void addPeerConnection(const PeerInfo &peer);


    const int8_t *getPeerID() const;

    const int8_t *getInfoHash() const;

    int getNumBytesUploaded() const;

    int getNumBytesDownloaded() const;

    int getNumBytesLeft() const;

    void start();

    void getDataFromPeers();
};


#endif //TORRENT_CLIENT_HPP
