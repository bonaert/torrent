#ifndef TORRENT_CLIENT_HPP
#define TORRENT_CLIENT_HPP

#include <string>
#include "Torrent.hpp"
#include "Tracker.hpp"

const int DEFAULT_PORT = 6881;

class Client {
    Torrent torrent;
    std::string peerID;
    int port;

    /* Tracker information */
    TrackerResponse * latestTrackerResponse;
    std::string trackerID;


    int numBytesUploaded;
    int numBytesDownloaded;
    int numBytesLeft;


    std::vector<Peer> peers;


public:
    Client(std::string filename);

    /* Trackers communication */
    void sendGetPeersRequestToTracker(const std::string &event);
    void sendGetPeersRequestToTracker();

    void processGetPeersResponseFromTracker(const std::string& response);




    std::string buildPeerID();

    void sendRequest(std::string basic_string);

    void updatePeers();
};


#endif //TORRENT_CLIENT_HPP