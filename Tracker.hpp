#ifndef TORRENT_TRACKER_HPP
#define TORRENT_TRACKER_HPP

#include <string>
#include "Torrent.hpp"

const std::vector<std::string> ACCEPTED_EVENTS = {"started", "completed", "stopped"};

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

    std::string urlencode(const std::string &basic_string);
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



    TrackerResponse(const std::string &response);
    TrackerResponse(const BDictionary &response);

    void urldecode(const std::string &basic_string);
};


#endif //TORRENT_TRACKER_HPP
