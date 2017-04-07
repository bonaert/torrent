#ifndef TORRENT_CLIENT_HPP
#define TORRENT_CLIENT_HPP

#include <string>
#include "Torrent.hpp"
#include "Tracker.hpp"
#include "Utils/UDPCommunicator.hpp"
#include "TrackerMaster.hpp"

const int DEFAULT_PORT = 6881;

class PeerInfo;

class TrackerMaster;

class Client {
    Torrent torrent;
    TrackerMaster trackerMaster;
    int8_t peerID[20];

    /* Tracker information */
//    TrackerResponse * latestTrackerResponse;
//    std::string trackerID;


    int numBytesUploaded;
    int numBytesDownloaded;
    int numBytesLeft;


    std::vector<PeerInfo> peers;


    void buildPeerID();

public:
    Client(std::string &filename);

    Client(Torrent &torrent);

    /* Trackers communication */
    void sendGetPeersRequestToTracker(const std::string &event);
    void sendGetPeersRequestToTracker();

    bool sendGetPeersUDPRequestToTracker(const std::string &event);

    void processGetPeersResponseFromTracker(const std::string& response);





    void sendRequest(std::string basic_string);

    void updatePeers();

    void sendGetPeersRequestToTracker(const std::string &announce, const std::string &event);

    void sendGetPeersUDPRequestToTracker();


    const int8_t *getPeerID() const;

    const int8_t *getInfoHash() const;


    int getNumBytesUploaded() const;

    int getNumBytesDownloaded() const;

    int getNumBytesLeft() const;

    void addPeer(PeerInfo &peer);

    void setup();
};


#endif //TORRENT_CLIENT_HPP
