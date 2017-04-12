#include <iostream>
#include "PeerManager.hpp"

#include "../Client.hpp"
#include "../Utils/Networking.hpp"

PeerManager::PeerManager(Client *client) :
        numTasksAdded(0),
        client(client),
        threadPool(DEFAULT_PEER_THREADS) {
}

void PeerManager::handleNewPeerFound(PeerInfo peer) {
    if (!peerConnectionAlreadyExists(peer)) {
        threadPool.enqueue(PeerManager::startConnection, peer, numTasksAdded++, this);
    }
}

void PeerManager::startConnection(PeerInfo peer, int numTask, PeerManager *peerManager) {
    PeerConnection connection(peer, peerManager);
    connection.connectToPeer();
    sleep(2);
}

const int8_t *PeerManager::getInfoHash() {
    return client->getInfoHash();
}

const int8_t *PeerManager::getPeerID() {
    return client->getPeerID();
}

bool PeerManager::peerConnectionAlreadyExists(const PeerInfo &info) {
    // TODO
    return false;
}





