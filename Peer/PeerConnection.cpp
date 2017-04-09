#include <cstring>
#include "PeerConnection.hpp"
#include "../Utils/Networking.hpp"
#include "PeerManager.hpp"
#include "../Torrent.hpp"

/* Handshake */

const std::string PROTOCOL_STRING = "BitTorrent protocol";
const int PROTOCOL_STRING_SIZE = (const int) PROTOCOL_STRING.length();
const char RESERVED_BYTES[8] = {0, 0, 0, 0, 0, 0, 0, 0};

const std::string COMMON_MESSAGE =
        (char) PROTOCOL_STRING_SIZE + // The length must be encoded as a single byte
        PROTOCOL_STRING +
        RESERVED_BYTES; // TODO: check that this line is going to work


std::string makeHandshakeMessage(const std::string &infoHash, const std::string &peerID) {
    return COMMON_MESSAGE + infoHash + peerID;
}

std::string makeHandshakeMessage(const int8_t *infoHash, const int8_t *peerID) {
    return COMMON_MESSAGE
           + std::string((char *) infoHash, HASH_SIZE)
           + std::string((char *) peerID, HASH_SIZE);
}


/* Messages */

std::string formatInt(unsigned int x) {
    // TODO: It must be big endian
    return std::to_string(x);
}


// TODO: most of these are stupid, because there's tons of useless copying around
// A better thought system would do this very differently, but for now it doesn't matter

// bitfield message: <len=0001+X><id=5><bitfield>
// X = size of the bitfield
std::string makeBitfieldMessage(char *bitfield, int size) {
    std::string message = formatInt((unsigned int) (1 + size));
    message += (char) (5);
    message += bitfield;
    return message;
}

// request: <len=0013><id=6><index><begin><length>
std::string makeRequestMessage(int pieceIndex, int offset, int length) {
    char message[4 + 13 + 1];
    message[3] = 13; // Message length
    message[4] = 6;  // Message ID
    *((int *) (&(message[5]))) = pieceIndex;
    *((int *) (&(message[9]))) = offset;
    *((int *) (&(message[13]))) = length;
    message[18] = '\0';
    return std::string(message);
}

// piece: <len=0009+X><id=7><index><begin><block>
// X = size of the block
std::string makePieceMessage(int pieceIndex, int offset, int blockLength, char *block) {
    std::string message = formatInt((unsigned int) (9 + blockLength));
    message += (char) 7; // ID
    message += pieceIndex;
    message += offset;
    message += std::string(block, (unsigned long) blockLength);
    return message;
}

// cancel: <len=0013><id=8><index><begin><length>
std::string makeCancelMessage(int pieceIndex, int offset, int length) {
    char message[4 + 13 + 1];
    message[3] = 13; // Message length
    message[4] = 8;  // Message ID
    *((int *) (&(message[5]))) = pieceIndex;
    *((int *) (&(message[9]))) = offset;
    *((int *) (&(message[13]))) = length;
    message[18] = '\0';
    return std::string(message);
}

// port: <len=0003><id=9><listen-port>
char *makePortMessage(int port);

PeerConnection::PeerConnection(PeerInfo peerInfo, PeerManager *peerManager) :
        peerInfo(peerInfo),
        peerManager(peerManager),
        socket(-1) {

}

bool PeerConnection::connect() {
    int timeout = 5;
    socket = initConnectionToServer((uint32_t) peerInfo.ip, peerInfo.port, timeout);
    return (socket != -1);
}

bool const PeerConnection::operator<(const PeerConnection &other) const {
    return peerInfo < other.peerInfo;
}

bool PeerConnection::doHandshake() {
    bool success = sendHandshake();
    if (!success) return false;
    return receiveHandshake();
}

bool PeerConnection::sendHandshake() {
    const int8_t *infoHash = peerManager->getInfoHash();
    const int8_t *peerID = peerManager->getPeerID();
    const std::string &message = makeHandshakeMessage(infoHash, peerID);
    return sendMessageToPeer(message);
}

bool PeerConnection::sendMessageToPeer(const std::string &message) {
    int bytesSent = sendData(socket, message.c_str(), (int) message.size());
    return bytesSent == message.size();
}

bool PeerConnection::connectToPeer() {
    bool couldConnect = connect();
    if (!couldConnect) return false;
    return doHandshake();
}

bool PeerConnection::receiveHandshake() {
    return false;
}
