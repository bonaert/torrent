#include <iostream>
#include "Client.hpp"
#include <curl/curl.h>


/*
 * http://www.bittorrent.org/beps/bep_0015.html
 */


Client::Client(std::string &filename) : torrent(filename),
                                        peerID(buildPeerID()),
                                        port(DEFAULT_PORT) {
    numBytesUploaded = 0;
    numBytesDownloaded = 0;
    numBytesLeft = torrent.size();
}

Client::Client(Torrent &torrent) : torrent(torrent),
                                   peerID(buildPeerID()),
                                   port(DEFAULT_PORT) {
    numBytesUploaded = 0;
    numBytesDownloaded = 0;
    numBytesLeft = torrent.size();
}

void Client::sendGetPeersRequestToTracker() {
    sendGetPeersRequestToTracker("");
}

void Client::sendGetPeersRequestToTracker(const std::string &event) {
    for (std::string &annouceUrl : torrent.metaInfo.announceList) {
        sendGetPeersRequestToTracker(annouceUrl, event);
    }
}

void Client::sendGetPeersRequestToTracker(const std::string &announce, const std::string &event) {
//    std::string protocolLessURL = announce.substr(6, announce.size() - 6);

    TrackerRequest request(announce);
    std::string requestMessage = request
            .addInfoHash(torrent.metaInfo.infoDictHash)
            .addPeerID(peerID)
            .addPort(port)
            .addUploadedBytes(numBytesUploaded)
            .addDownloadedBytes(numBytesDownloaded)
            .addBytesLeft(numBytesLeft)
            .addAcceptsCompactResponseFlag(false)
            .addEvent(event)
            .getRequestURL();

    sendRequest(requestMessage);
}

void Client::processGetPeersResponseFromTracker(const std::string &response) {
    if (latestTrackerResponse == nullptr) {
        latestTrackerResponse = buildTrackerResponse(response);
    } else {
        *latestTrackerResponse = *buildTrackerResponse(response);
    }


    /* Handle error cases */
    if (latestTrackerResponse->failed) {
        std::cout << "The tracker couldn't send peers for the following reason:" << std::endl;
        std::cout << latestTrackerResponse->failureReason << std::endl;
        return;
    }

    if (latestTrackerResponse->warning.length() != 0) {
        std::cout << "The tracker sent the following warning:" << std::endl;
        std::cout << latestTrackerResponse->warning << std::endl;
    }



    /* Process response */
    if (latestTrackerResponse->trackerID.size() != 0) {
        trackerID = latestTrackerResponse->trackerID; // Update the tracker ID
    }

    updatePeers();
}


void Client::updatePeers() {
    // TODO: for now, it only appends the new peers
    // However, there could be duplicates, which is bad, so later I'll have to fix
    // This could be done easily by using a set, and implementing a comparison
    // operation on the peers that only compares their peer ID

    for (const Peer &peer : latestTrackerResponse->peers) {
        peers.push_back(peer);
    }
}


/*
 * UTILITIES
 */


std::string Client::buildPeerID() {
    std::string ID = "-UT0001-";
    for (int i = 0; i < 12; ++i) { // Add 12 random bytes
        ID += (char) rand();
    }
    return ID;
}


/*
 * NETWORKING
 */


void Client::sendRequest(std::string url) {
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_LOCALPORT, 6881);

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);

        /* Check for errors */
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        /* always cleanup */
        curl_easy_cleanup(curl);
    }
}




