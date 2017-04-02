#include <unistd.h>
#include <iostream>
#include "Torrent.hpp"

/*
 * The following examples methods demonstrate how the API should be used.
 * This is here to help design the API of the library, and make sure the
 * interfaces are as simple and clear as possible.
 */

void downloadTorrent(std::string filename) {
    Torrent torrent = Torrent(filename);
    float totalSize = torrent.size();
    int numPieces = torrent.numPieces();
    int pieceSize = torrent.pieceSize();
    std::string name = torrent.name();


    torrent.start(); // Starts the torrent in a new thread?

    while (!torrent.hasFinished()){
        std::cout << "Here are the stats for the torrent \"" << name << "\"" << std::endl;

        float percentComplete = torrent.percentComplete();
        std::cout << "The torrent is " << (int) percentComplete << " % complete" << std::endl;

        int bytesDownloaded = torrent.bytesDownloaded();
        std::cout << "Downloaded: " << bytesDownloaded << " / " << totalSize << std::endl;

        int numActivePeers = torrent.numActivePeers();
        std::cout << "Sharing data with " << numActivePeers << " peers." << std::endl;

        sleep(10);
    }

    std::cout << "The torrent is now complete!" << std::endl;
    std::cout << "You can check your file at " << name << std::endl;
}

int main() {
    std::cout << "Hello, World!" << std::endl;
    downloadTorrent("file.torrent");
    return 0;
}