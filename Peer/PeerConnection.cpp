#include <cstring>
#include "PeerConnection.hpp"

/* Handshake */

const std::string PROTOCOL_STRING = "BitTorrent protocol";
const int PROTOCOL_STRING_SIZE = (const int) PROTOCOL_STRING.length();
const char RESERVED_BYTES[8] = {0, 0, 0, 0, 0, 0, 0, 0};

const std::string COMMON_MESSAGE =
        "handshake:" + // TODO: not sure this is part of the message
        (char) PROTOCOL_STRING_SIZE + // The length must be encoded as a single byte
        PROTOCOL_STRING +
        RESERVED_BYTES; // TODO: check that this line is going to work


std::string makeHandshakeMessage(const std::string &infoHash, const std::string &peerID) {
    return COMMON_MESSAGE + infoHash + peerID;
}


/* Messages */

std::string formatInt(unsigned int x){
    // TODO: It must be big endian
    return std::to_string(x);
}


// TODO: most of these are stupid, because there's tons of useless copying around
// A better thought system would do this very differently, but for now it doesn't matter

// bitfield message: <len=0001+X><id=5><bitfield>
// X = size of the bitfield
std::string makeBitfieldMessage(char * bitfield, int size) {
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
std::string makePieceMessage(int pieceIndex, int offset, int blockLength, char * block){
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
char * makePortMessage(int port);
