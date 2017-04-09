#include <unistd.h>
#include "PeerManager.hpp"
#include "../Client.hpp"
#include "../Utils/Tools.hpp"

PeerManager::PeerManager(Client *client) : client(client) {

}

void PeerManager::addPeer(const PeerInfo &peer) {
    peerQueue.push_back(peer);
}

void PeerManager::start() {
    startPeerConnections();
}

void PeerManager::startPeerConnections() {
    int numThreadsToStart = min((int) peerQueue.size(),
                                (int) (MAX_THREADS - threads.size()));

    auto it = peerQueue.begin();
    for (int i = 0; i < numThreadsToStart; ++i) {
        PeerConnection peerConnection(*it, this);
        peerConnections.push_back(peerConnection);

        std::thread thread(&PeerConnection::connectToPeer, peerConnection);
        thread.detach();
        threads.push_back(std::move(thread));

        // TODO: check whether the thread is going to be destroyed in the next iteration
        // or if it's preserved in the threads vector

        peerQueue.erase(it);
    }

    sleep(100);
    for (auto &&thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

const int8_t *PeerManager::getInfoHash() {
    return client->getInfoHash();
}

const int8_t *PeerManager::getPeerID() {
    return client->getPeerID();
}

const std::vector<PeerConnection> &PeerManager::getPeerConnections() const {
    return peerConnections;
}




