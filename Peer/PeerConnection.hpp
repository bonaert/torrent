#ifndef TORRENT_PEER_HPP
#define TORRENT_PEER_HPP

#include <string>
#include "../Tracker/Tracker.hpp"

/*
 * A block is downloaded by the client when the client is interested in a peer, and
 * that peer is not choking the client. A block is uploaded by a client when the client is
 * not choking a peer, and that peer is interested in the client.
 *
 * It is important for the client to keep its peers informed as to whether or not it
 * is interested in them. This state information should be kept up-to-date with each peer
 * even when the client is choked. This will allow peers to know if the client will
 * begin downloading when it is un-choked (and vice-versa).
 */

class PeerManager;

class PeerConnection {
private:
    bool amChoking = true;      // this client is choking the peer
    bool amInterested = false;   // this client is interested in the peer
    bool peerChoking = true;    // peer is choking this client
    bool peerInterested = false; // peer is interested in this client
    PeerInfo peerInfo;
    PeerManager *peerManager;
    int socket;


    bool connect();

    bool doHandshake();

    bool sendHandshake();

    bool receiveHandshake();


public:
    PeerConnection(PeerInfo peerInfo, PeerManager *peerManager);

    bool connectToPeer();

    // Need to define this to put PeerConnection in a std::set
    bool const operator<(const PeerConnection &other) const;

    bool sendMessageToPeer(const std::string &message);
};



/*
 * Handshake
 */

std::string makeHandshakeMessage(const std::string &infoHash, const std::string &peerID);

std::string makeHandshakeMessage(const int8_t *infoHash, const int8_t *peerID);

/*
 * Messages
 */

/*
 * All of the remaining messages in the protocol take the form of
 * <length prefix><message ID><payload>.
 * The length prefix is a four byte big-endian value.
 * The message ID is a single decimal byte.
 */

// keep-alive: <len=0000>
const char keepAliveMessage[4] = {0, 0, 0, 0};

// choke: <len=0001><id=0>
const char chokeMessage[5] = {0, 0, 0, 1, 0};

// unchoke: <len=0001><id=1>
const char unchokeMessage[5] = {0, 0, 0, 1, 1};

// interested: <len=0001><id=2>
const char interestedMessage[5] = {0, 0, 0, 1, 2};

// not interested: <len=0001><id=3>
const char notInterestedMessage[5] = {0, 0, 0, 1, 3};

// bitfield message: <len=0001+X><id=5><bitfield>
// X = size of the bitfield
std::string makeBitfieldMessage(char * bitfield, int size);

// request: <len=0013><id=6><index><begin><length>
std::string makeRequestMessage(int pieceIndex, int offset, int length);

// piece: <len=0009+X><id=7><index><begin><block>
// X = size of the block
std::string makePieceMessage(int pieceIndex, int offset, int blockLength, char * block);

// cancel: <len=0013><id=8><index><begin><length>
std::string makeCancelMessage(int pieceIndex, int offset, int length);

// port: <len=0003><id=9><listen-port>
char * makePortMessage(int port);


// TODO: make sure I respect this sentence
/*
 * Unless specified otherwise, all integers in the peer wire protocol are encoded
 * as four byte big-endian values. This includes the length prefix on all messages
 * that come after the handshake.
 */

// TODO: don't forget to use types with explicit size (e.g. int32_t) because
// the message size needs to match the message length in the start of the requests

#endif //TORRENT_PEER_HPP
