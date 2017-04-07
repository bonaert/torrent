#ifndef TORRENT_TRACKER_HPP
#define TORRENT_TRACKER_HPP

#include <string>
#include "Torrent.hpp"
#include "Utils/UDPCommunicator.hpp"
#include "Client.hpp"

const std::vector<std::string> ACCEPTED_EVENTS = {"", "started", "completed", "stopped"};


/*
 *
 * TRACKER UDP REQUEST
 *
 * http://www.bittorrent.org/beps/bep_0015.html
 *
 */

class Client;

static const int DEFAULTPORT = 6881;
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

typedef struct PeerInfo {
    int32_t ip;
    uint16_t port;
} PeerInfo;

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
    int32_t numPeersWanted = -1;
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

class UDPTracker {
private:
    Client *client;
    UDPCommunicator *udpCommunicator;
    std::vector<PeerInfo> peers;


    int32_t transactionID;
    int64_t connectionID;

    bool connectToServer();

    bool sendConnectRequest();

    bool processConnectResponse();

    bool getPeersFromServer();

    bool sendGetPeersRequest();

    bool processGetPeersResponse();

public:
    UDPTracker(Client *client, const std::string &announceURL);

    void getPeers();

    AnnounceRequest buildAnnounceRequest() const;

    void writeAnnounceRequestToBuffer(AnnounceRequest &request, char **pString, int *pInt);
};


/*
 * TRACKER HTTP REQUEST (LEGACY)
 */

class TrackerRequest {
private:
    std::string requestString;
    bool addedFirstParameter;

    void addKeyInRequest(const std::string& key);

    void addKeyValuePairInRequest(const std::string& key, const std::string& value);

    void addUrlEncodedKeyInRequest(const std::string& key);

    void addUrlEncodedKeyValuePairInRequest(const std::string& key, const std::string& value);


public:
    TrackerRequest(std::string announceUrl);

    TrackerRequest& addInfoHash(const std::string& infoHash);

    TrackerRequest& addPeerID(const std::string& peerID);

    TrackerRequest& addPort(int port);

    TrackerRequest& addUploadedBytes(int uploadedBytes);

    TrackerRequest& addDownloadedBytes(int downloadedBytes);

    TrackerRequest& addBytesLeft(int bytesLeft);

    TrackerRequest& addAcceptsCompactResponseFlag(bool compactFlag);

    TrackerRequest& addNoPeerIDFlag();

    TrackerRequest& addEvent(const std::string& event);

    TrackerRequest& addIP(const std::string& ip);

    TrackerRequest& addNumberPeersWanted(int numPeersWanted);

    TrackerRequest& addKey(const std::string& key);

    TrackerRequest& addTrackerID(const std::string& trackerID);

    const std::string& getRequestURL();
};

typedef struct Peer {
    std::string peerID;
    std::string ip;
    int port;
} Peer;


class TrackerResponse {
private:
    void parseResponse(const BDictionary &response);
public:
    bool failed;
    std::string failureReason;
    std::string warning;

    int intervalBetweenRequests;
    int minimumIntervalBetweenRequests = -1;

    std::string trackerID;
    int numSeeders;
    int numLeechers;
    std::vector<Peer> peers;



    TrackerResponse(const BDictionary &response);
};

TrackerResponse *buildTrackerResponse(const std::string &response);

#endif //TORRENT_TRACKER_HPP
