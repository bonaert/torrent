#include <assert.h>
#include <algorithm>
#include "Tracker.hpp"


bool contains(const std::vector<std::string>& vector, const std::string& element){
    return std::find(vector.begin(), vector.end(), element) != vector.end();
}


/*
 *
 * TRACKER REQUEST
 *
 */


TrackerRequest::TrackerRequest(std::string announceUrl): requestString(announceUrl + '?'),
                                                         addedFirstParameter(false)
{
}

/*
 * UTILITY METHODS TO ADD PARAMETERS TO THE REQUEST STRING
 */

void TrackerRequest::addKeyInRequest(const std::string &key) {
    if (addedFirstParameter) {
        requestString += '&';
    } else {
        addedFirstParameter = true;
    }
    requestString += key;
}

void TrackerRequest::addKeyValuePairInRequest(const std::string &key, const std::string &value) {
    if (addedFirstParameter) {
        requestString += '&';
    } else {
        addedFirstParameter = true;
    }
    requestString += key + '=' + value;
}

void TrackerRequest::addUrlEncodedKeyInRequest(const std::string &key) {
    if (addedFirstParameter) {
        requestString += '&';
    } else {
        addedFirstParameter = true;
    }
    requestString += urlencode(key);
}

void TrackerRequest::addUrlEncodedKeyValuePairInRequest(const std::string &key, const std::string &value) {
    if (addedFirstParameter) {
        requestString += '&';
    } else {
        addedFirstParameter = true;
    }
    requestString += urlencode(key) + '=' + urlencode(value);
}


TrackerRequest &TrackerRequest::addInfoHash(const std::string& infoHash) {
    assert(infoHash.length() == HASH_SIZE);
    addKeyValuePairInRequest("info_hash", infoHash);
    return *this;
}

TrackerRequest &TrackerRequest::addPeerID(const std::string &peerID) {
    assert(peerID.length() == HASH_SIZE);
    addKeyValuePairInRequest("peer_id", peerID);
    return *this;
}

TrackerRequest &TrackerRequest::addPort(int port) {
    addKeyValuePairInRequest("port", std::to_string(port));
    return *this;
}

TrackerRequest &TrackerRequest::addUploadedBytes(int uploadedBytes) {
    addKeyValuePairInRequest("uploaded", std::to_string(uploadedBytes));
    return *this;
}

TrackerRequest &TrackerRequest::addDownloadedBytes(int downloadedBytes) {
    addKeyValuePairInRequest("downloaded", std::to_string(downloadedBytes));
    return *this;
}

TrackerRequest &TrackerRequest::addBytesLeft(int bytesLeft) {
    addKeyValuePairInRequest("left", std::to_string(bytesLeft));
    return *this;
}

TrackerRequest &TrackerRequest::addAcceptsCompactResponseFlag(bool compactFlag) {
    if (compactFlag){
        addKeyValuePairInRequest("compact", "1");
    } else {
        addKeyValuePairInRequest("compact", "0");
    }
    return *this;
}

TrackerRequest &TrackerRequest::addNoPeerIDFlag() {
    addKeyInRequest("no_peer_id");
    return *this;
}

TrackerRequest &TrackerRequest::addEvent(const std::string &event) {
    assert(contains(ACCEPTED_EVENTS, event));
    addKeyValuePairInRequest("event", event);
    return *this;
}

TrackerRequest &TrackerRequest::addIP(const std::string &ip) {
    addKeyValuePairInRequest("ip", ip);
    return *this;
}

TrackerRequest &TrackerRequest::addNumberPeersWanted(int numPeersWanted) {
    addKeyValuePairInRequest("numwant", std::to_string(numPeersWanted));
    return *this;
}

TrackerRequest &TrackerRequest::addKey(const std::string &key) {
    addUrlEncodedKeyValuePairInRequest("key", key);
    return *this;
}

TrackerRequest &TrackerRequest::addTrackerID(const std::string &trackerID) {
    addUrlEncodedKeyValuePairInRequest("trackerid", trackerID);
    return *this;
}

const std::string &TrackerRequest::getRequestURL() {
    return requestString;
}



/*
 *
 * TRACKER RESPONSE
 *
 */



TrackerResponse::TrackerResponse(const std::string &response): TrackerRequest(decode(response)) {
}

TrackerResponse::TrackerResponse(const BDictionary &response) {
    parseResponse(response);
}

void TrackerResponse::parseResponse(const BDictionary & response) {
    if (response.contains("failure reason")){
        failed = true;
        failureReason = response.getString("failure reason");
        return;
    } else {
        failed = false;
    }


    if (response.contains("warning message")) {
        warning = response.getString("warning message");
    }


    intervalBetweenRequests = response.getInt("interval");
    if (response.contains("min interval")) {
        minimumIntervalBetweenRequests = response.getInt("min interval");
    }

    if (response.contains("tracker id")) {
        trackerID = response.getString("tracker id");
    }

    numSeeders = response.getInt("complete");
    numLeechers = response.getInt("incomplete");

    /* Parse peers list */
    // TODO: maybe parse binary format, as described in
    // https://wiki.theory.org/BitTorrentSpecification#Tracker_Response
    // Currently we're assuming we'll only receive the dictionary model of peers


    const BList &peersList = response.getList("peers");
    for (int i = 0; i < peersList.size(); ++i) {
        const BDictionary& peerDict = peersList.getDictionary(i);
        Peer peer;
        peer.peerID = peerDict.getString("peer id");
        peer.ip = peerDict.getString("ip");
        peer.port = peerDict.getInt("port");
        peers.push_back(peer);
    }
}