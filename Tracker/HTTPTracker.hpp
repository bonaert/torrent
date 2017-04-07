#ifndef TORRENT_HTTPTRACKER_HPP
#define TORRENT_HTTPTRACKER_HPP

#include <string>
#include <vector>
#include "Tracker.hpp"
#include "../BEncode.hpp"


class HTTPTracker : public Tracker {
private:
    std::string announceUrl;

    bool sendRequest(std::string url);

    bool sendGetPeersRequest();

    bool sendGetPeersRequest(const std::string &event);

    bool getResponseFromTracker();

    void processGetPeersResponseFromTracker(const std::string &response);

public:
    HTTPTracker(TrackerMaster *trackerMaster, const std::string &announceURL);

    void updatePeers() override;


};

/*
 * TRACKER HTTP REQUEST (LEGACY)
 */

class TrackerRequest {
private:
    std::string requestString;
    bool addedFirstParameter;

    void addKeyInRequest(const std::string &key);

    void addKeyValuePairInRequest(const std::string &key, const std::string &value);

    void addUrlEncodedKeyInRequest(const std::string &key);

    void addUrlEncodedKeyValuePairInRequest(const std::string &key, const std::string &value);


public:
    TrackerRequest(std::string announceUrl);

    TrackerRequest &addInfoHash(const std::string &infoHash);

    TrackerRequest &addPeerID(const std::string &peerID);

    TrackerRequest &addPort(int port);

    TrackerRequest &addUploadedBytes(int uploadedBytes);

    TrackerRequest &addDownloadedBytes(int downloadedBytes);

    TrackerRequest &addBytesLeft(int bytesLeft);

    TrackerRequest &addAcceptsCompactResponseFlag(bool compactFlag);

    TrackerRequest &addNoPeerIDFlag();

    TrackerRequest &addEvent(const std::string &event);

    TrackerRequest &addIP(const std::string &ip);

    TrackerRequest &addNumberPeersWanted(int numPeersWanted);

    TrackerRequest &addKey(const std::string &key);

    TrackerRequest &addTrackerID(const std::string &trackerID);

    const std::string &getRequestURL();
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


#endif //TORRENT_HTTPTRACKER_HPP
