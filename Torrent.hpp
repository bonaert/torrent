#ifndef TORRENT_TORRENT_HPP
#define TORRENT_TORRENT_HPP

#include <string>
#include <vector>

const int HASH_SIZE = 20;

typedef struct FileInfo {
    int numBytes;
    std::string path;
} FileInfo ;

typedef struct MetaInfo {
    // Top level info
    std::string announceURL;
    int creationDate = -1;
    std::string comment = "";
    std::string createdBy = "";
    std::string encoding = "";

    // Info dictionary
    char infoDictHash[HASH_SIZE];
    int pieceLength;
    int numPieces;
    char piecesHash[][HASH_SIZE];  // Array of 20 char hashes for each of the pieces
    bool isPrivate = false;

    // File information
    std::string directoryName;
    std::vector<FileInfo> fileInfos;
} MetaInfo;


class Torrent {
private:
    void parseTorrentFile();

public:
    std::string torrentFileName;
    MetaInfo metaInfo;



    Torrent(std::string torrentFileName);

    int size();

    int numPieces();

    int pieceSize();

    std::string name();

    void start();

    bool hasFinished();

    float percentComplete();

    int bytesDownloaded();

    int numActivePeers();

    std::string buildPath(const BEncodedDictionary &pathDictionary) const;
};


#endif //TORRENT_TORRENT_HPP
