# torrent

I wanted to learn how Bittorrent works, so I made the simplest and clearest Bittorrent client I could.
Only the the minimal amount of stuff needed to make it usable has been implemented.
The goal was to make the code clear, simple and as easy to read and use as possible.

The [main.cpp](https://github.com/harokb/torrent/blob/master/main.cpp) file contains an example of how the Torrent class can be used. It's still in progress, so the API may change.
Here's some sample code:

```cpp
Torrent torrent = Torrent(filename);
float totalSize = torrent.size();
int numPieces = torrent.numPieces();
int pieceSize = torrent.pieceSize();
std::string name = torrent.name();

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

std::cout << "The torrent is now complete!" << std::endl;
std::cout << "You can check your file at " << name << std::endl;
```

If you want to learn more, read the code. The [Bittorrent protocol](https://wiki.theory.org/BitTorrentSpecification) is surprisingly short 
and easy to understand. If you want to read it alongside the code, you can find it [here](https://wiki.theory.org/BitTorrentSpecification).
Of course, in real life, use [libtorrent](https://github.com/arvidn/libtorrent).
