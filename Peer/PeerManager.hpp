#ifndef TORRENT_PEERMANAGER_HPP
#define TORRENT_PEERMANAGER_HPP

#include <thread>
#include <vector>
#include "../Tracker/Tracker.hpp"
#include "PeerConnection.hpp"

/*
 * PeerManager controls the different PeerConnections. It's here to decide
 * when to start new connections, when to discard old ones, and to deal with
 * all the thread that are needed to make this work.
 */
class PeerManager {
private:
    std::vector<std::thread> threads;
    std::vector<PeerConnection> peerConnections;
public:
    const std::vector<PeerConnection> &getPeerConnections() const;

private:
    std::vector<PeerInfo> peerQueue;
    const int MAX_THREADS = 20;
    Client *client;

    void startPeerConnections();

public:
    PeerManager(Client *client);

    void addPeer(const PeerInfo &peer);

    const int8_t *getInfoHash();

    const int8_t *getPeerID();

    void start();
};


#endif //TORRENT_PEERMANAGER_HPP
