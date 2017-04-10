#include "PeerManager.hpp"
#include "../Client.hpp"
#include "../Utils/Networking.hpp"

PeerManager::PeerManager(Client *client) :
        client(client),
        threadPool(DEFAULT_PEER_THREADS) {
    start();
}

void PeerManager::addPeerToQueue(PeerInfo peer) {
    threadPool.postTask(&PeerManager::startConnection, peer);
}

void PeerManager::startConnection(PeerInfo peer) {
    std::cout << "Added peer " << getHumanReadableIP(peer.ip) << " . Sleeping." << std::endl;
    sleep(5);
    std::cout << "Finished sleeping! The job is complete";
}

void PeerManager::start() {
    threadPool.start();
}

const int8_t *PeerManager::getInfoHash() {
    return client->getInfoHash();
}

const int8_t *PeerManager::getPeerID() {
    return client->getPeerID();
}





