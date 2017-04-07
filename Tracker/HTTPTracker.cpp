#include <assert.h>
#include <sstream>
#include <curl/curl.h>
#include "HTTPTracker.hpp"
#include "../Torrent.hpp"
#include "../Utils/Tools.hpp"

/*
 *
 * TRACKER HTTP REQUEST (LEGACY)
 *
 */


TrackerRequest::TrackerRequest(std::string announceUrl) : requestString(announceUrl + '?'),
                                                          addedFirstParameter(false) {
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


TrackerRequest &TrackerRequest::addInfoHash(const std::string &infoHash) {
    assert(infoHash.length() == HASH_SIZE);
    addUrlEncodedKeyValuePairInRequest("info_hash", infoHash);
    return *this;
}

TrackerRequest &TrackerRequest::addPeerID(const std::string &peerID) {
    assert(peerID.length() == HASH_SIZE);
    addUrlEncodedKeyValuePairInRequest("peer_id", peerID);
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
    if (compactFlag) {
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


TrackerResponse *buildTrackerResponse(const std::string &response) {
    std::istringstream stream(response);

    BDictionary *responseDict = static_cast<BDictionary *>(parseBItem(stream));
    TrackerResponse *trackerResponse = new TrackerResponse(*responseDict);
    delete responseDict;

    return trackerResponse;
}

TrackerResponse::TrackerResponse(const BDictionary &response) {
    parseResponse(response);
}

void TrackerResponse::parseResponse(const BDictionary &response) {
    if (response.contains("failure reason")) {
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
        const BDictionary &peerDict = peersList.getDictionary(i);
        Peer peer;
        peer.peerID = peerDict.getString("peer id");
        peer.ip = peerDict.getString("ip");
        peer.port = peerDict.getInt("port");
        peers.push_back(peer);
    }
}






HTTPTracker::HTTPTracker(TrackerMaster *trackerMaster, const std::string &announceURL) :
        Tracker(trackerMaster), announceUrl(announceURL) {
}

void HTTPTracker::updatePeers() {
    bool success = sendGetPeersRequest();
    if (success) {
        getResponseFromTracker();
    }
}

bool HTTPTracker::sendGetPeersRequest() {
    return sendGetPeersRequest("");
}

bool HTTPTracker::sendGetPeersRequest(const std::string &event) {
//    std::string protocolLessURL = announce.substr(6, announce.size() - 6);
    TrackerRequest request(announceUrl);
    const int8_t *hash = trackerMaster->getInfoHash();
    std::string hashString = std::string((char *) hash, 20);

    const int8_t *peerID = trackerMaster->getPeerID();
    std::string peerIDString = std::string((char *) peerID, 20);

    std::string requestMessage = request
            .addInfoHash(hashString)
            .addPeerID(peerIDString)
            .addPort(-1) // BUG
            .addDownloadedBytes((int) trackerMaster->getNumBytesDownloaded())
            .addUploadedBytes((int) trackerMaster->getNumBytesUploaded())
            .addBytesLeft((int) trackerMaster->getNumBytesLeft())
            .addAcceptsCompactResponseFlag(false)
            .addEvent(event)
            .getRequestURL();

    return sendRequest(requestMessage);
}

bool HTTPTracker::getResponseFromTracker() {
    return false;
}


void HTTPTracker::processGetPeersResponseFromTracker(const std::string &response) {
    /*
    if (latestTrackerResponse == nullptr) {
        latestTrackerResponse = buildTrackerResponse(response);
    } else {
        *latestTrackerResponse = *buildTrackerResponse(response);
    }


    // Handle error cases
    if (latestTrackerResponse->failed) {
        std::cout << "The tracker couldn't send peers for the following reason:" << std::endl;
        std::cout << latestTrackerResponse->failureReason << std::endl;
        return;
    }

    if (latestTrackerResponse->warning.length() != 0) {
        std::cout << "The tracker sent the following warning:" << std::endl;
        std::cout << latestTrackerResponse->warning << std::endl;
    }



    // Process response
    if (latestTrackerResponse->trackerID.size() != 0) {
        trackerID = latestTrackerResponse->trackerID; // Update the tracker ID
    }

    updatePeers();
    */
}


/*
void Client::updatePeers() {
    // TODO: for now, it only appends the new peers
    // However, there could be duplicates, which is bad, so later I'll have to fix
    // This could be done easily by using a set, and implementing a comparison
    // operation on the peers that only compares their peer ID

    for (const Peer &peer : latestTrackerResponse->peers) {
        peers.push_back(peer);
    }

}
*/

/*
 * NETWORKING
 */


bool HTTPTracker::sendRequest(std::string url) {
    CURL *curl;
    CURLcode res;

    bool success = false;
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_LOCALPORT, DEFAULT_PORT);

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);

        /* Check for errors */
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\nHost: %s\n", curl_easy_strerror(res), url.c_str());
        } else {
            success = true;
        }

        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    return success;
}



