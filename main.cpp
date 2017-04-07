
#include <iostream>
#include <assert.h>
#include "Torrent.hpp"
#include "Utils/Tools.hpp"
#include "Client.hpp"

/*
 * The following examples methods demonstrate how the API should be used.
 * This is here to help design the API of the library, and make sure the
 * interfaces are as simple and clear as possible.
 */

void downloadTorrent(std::string filename) {
    Torrent torrent = Torrent(filename);
    int totalSize = torrent.size();
    int numPieces = torrent.numPieces();
    int pieceSize = torrent.pieceLength();
    std::string name = torrent.name();


    Client client(torrent);
    client.sendGetPeersUDPRequestToTracker();
/*
    Client client(torrent);
    client.start(); // Starts the torrent in a new thread [NOT SURE, MAY CHANGE]

    while (!client.hasFinished()){
        std::cout << "Here are the stats for the torrent \"" << name << "\"" << std::endl;

        float percentComplete = client.percentComplete();
        std::cout << "The torrent is " << (int) percentComplete << " % complete" << std::endl;

        int bytesDownloaded = client.bytesDownloaded();
        std::cout << "Downloaded: " << bytesDownloaded << " / " << totalSize << std::endl;

        int numActivePeers = client.numActivePeers();
        std::cout << "Sharing data with " << numActivePeers << " peers." << std::endl;

        sleep(10);
    }
*/

    std::cout << "The torrent is now complete!" << std::endl;
    std::cout << "You can check your file at " << name << std::endl;
}

void runTests() {
    assert(urlencode("Hello Günter") == "Hello%20G%C3%BCnter");
    assert(urldecode("Hello%20G%C3%BCnter") == "Hello Günter");
}

void setup() {
    srand((unsigned int) time(NULL));
}


int main() {
    setup();
    std::cout << "Hello, World!" << std::endl;
//    const char *filename = "/home/greg/Downloads/torrents/[kat.cr]silicon.valley.s03e07.hdtv.x264.killers.ettv.torrent";
    const char *filename = "/home/greg/Downloads/torrents/[kat.cr]shutter.island.2010.720p.1280.544.600mb.torrent";
    downloadTorrent(filename);

    runTests();

    return 0;
}