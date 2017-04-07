#ifndef TORRENT_UDP_TRACKER_HPP
#define TORRENT_UDP_TRACKER_HPP

#include <string>
#include "Tracker.hpp"
#include "../Torrent.hpp"
#include "../Utils/UDPCommunicator.hpp"


/*
 *
 * TRACKER UDP REQUEST
 *
 * http://www.bittorrent.org/beps/bep_0015.html
 *
 */

class Client;

// WARNING: make sure you initialize the seed once before using this!
struct ConnectRequest {
    int64_t protocolID = 0x41727101980; // Magic number
    int32_t action = 0;                 // Connect
    int32_t transactionID = rand();     // Random transaction ID for each transaction
};

void writeConnectRequestToBuffer(const ConnectRequest &request, char **bufferPointer, int *length);

struct ConnectResponse {
    ConnectResponse(char response[16]);

    int32_t action;
    int32_t transactionID;
    int64_t connectionID;
};

//ConnectResponse getConnectResponse();



struct AnnounceRequest {
    int64_t connectionID;
    int32_t action = 1;           // 1 = announce
    int32_t transactionID;
    int8_t infoHash[HASH_SIZE];
    int8_t peerID[HASH_SIZE];
    int64_t numBytesDownloaded;
    int64_t numBytesLeft;
    int64_t numBytesUploaded;
    int32_t event = 0;       // 0: none; 1: completed; 2: started; 3: stopped
    uint32_t IPAdress = 0;    // 0: default
    uint32_t key = 0;
    int32_t numPeersWanted = 50;
    uint16_t port;
    uint16_t extensions = 0;
};


//void sendAnnounceRequest(const AnnounceRequest& request);

struct AnnounceResponse {
    AnnounceResponse(char buffer[], int numChars);

    int32_t action; // 1 = announce
    int32_t transactionID;
    int32_t intervalBetweenAnnouceRequests;
    int32_t numLeechers;
    int32_t numSeeders;
    std::vector<PeerInfo> peers;
};

//AnnounceReponse getAnnounceReponse();

class UDPTracker : public Tracker {
private:
    UDPCommunicator *udpCommunicator;


    int32_t transactionID;
    int64_t connectionID;
    std::string trackerDomain;

    bool connectToServer();

    bool sendConnectRequest();

    bool processConnectResponse();

    bool getPeersFromServer();

    bool sendGetPeersRequest();

    bool processGetPeersResponse();

    AnnounceRequest buildAnnounceRequest() const;

    void writeAnnounceRequestToBuffer(AnnounceRequest &request, char **pString, int *pInt);


public:
    UDPTracker(TrackerMaster *trackerMaster, const std::string &announceURL);

    virtual void updatePeers();
};


#endif //TORRENT_UDP_TRACKER_HPP
