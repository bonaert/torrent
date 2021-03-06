#ifndef TORRENT_PEERMANAGER_HPP
#define TORRENT_PEERMANAGER_HPP

#include <vector>
#include "../Tracker/Tracker.hpp"
#include "PeerConnection.hpp"
//#include "../Utils/ThreadPool.hpp"
#include "../lib/threadpool/Threadpool.hpp"

const int DEFAULT_PEER_THREADS = 20;

/*
 * PeerManager controls the different PeerConnections. It's here to decide
 * when to start new connections, when to discard old ones, and to deal with
 * all the thread that are needed to make this work.
 */
class PeerManager {
private:
    ThreadPool threadPool;
    Client *client;
    int numTasksAdded;


public:
    PeerManager(Client *client);

    const int8_t *getInfoHash();

    const int8_t *getPeerID();

    void handleNewPeerFound(PeerInfo peer);

    static void startConnection(PeerInfo peer, int numTask, PeerManager *peerManager);

    bool peerConnectionAlreadyExists(const PeerInfo &info);
};


#endif //TORRENT_PEERMANAGER_HPP
