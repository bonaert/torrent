
#include <iostream>
#include "Torrent.hpp"
#include "Utils/Tools.hpp"

Torrent::Torrent(std::string torrentFileName) : torrentFileName(torrentFileName) {
    parseTorrentFile();
}

int Torrent::size() {
    return metaInfo.totalSize;
}

int Torrent::numPieces() {
    return metaInfo.numPieces;
}

int Torrent::pieceLength() {
    return (int) metaInfo.pieceLength;
}

void Torrent::start() {

}

float Torrent::percentComplete() {
    return 0;
}

bool Torrent::hasFinished() {
    return false;
}

int Torrent::numActivePeers() {
    return 0;
}

int Torrent::bytesDownloaded() {
    return 0;
}

std::string Torrent::name() {
    return metaInfo.directoryName;
}


/*
 *
 * PARSE THE .TORRENT FILE
 *
 */



void Torrent::parseTorrentFile() {
    std::ifstream torrentFile;
    torrentFile.open(torrentFileName);

    BDictionary metaInfoDict(torrentFile);

    /* Parse the meta info fields */
    metaInfo.announceURL = metaInfoDict.getString("announce");

    BList announceURLsList = metaInfoDict.getList("announce-list");
    for (int j = 0; j < announceURLsList.size(); ++j) {
        BList announceURLlist = announceURLsList.getList(j);
        std::string announceURL = announceURLlist.getString(0);
        metaInfo.announceList.push_back(announceURL);
//        announceURLList
    }

    if (metaInfoDict.contains("creation date")) {
        metaInfo.creationDate = (int) metaInfoDict.getInt("creation date");
    }

    if (metaInfoDict.contains("comment")) {
        metaInfo.comment = metaInfoDict.getString("comment");
    }

    if (metaInfoDict.contains("created by")) {
        metaInfo.createdBy = metaInfoDict.getString("created by");
    }

    if (metaInfoDict.contains("encoding")) {
        metaInfo.encoding = metaInfoDict.getString("encoding");
    }


    /* Parse the info dictionary */
    const BDictionary &infoDictionary = metaInfoDict.getDictionary("info");


    computeSHA1Hash((unsigned char *) metaInfo.infoDictHash, infoDictionary.encode());
    printHash();


    metaInfo.pieceLength = infoDictionary.getInt("piece length");
    if (infoDictionary.contains("private")) {
        metaInfo.isPrivate = (infoDictionary.getInt("private") == 1);
    }

    const std::string &hashes = infoDictionary.getString("pieces");
    metaInfo.numPieces = (int) (hashes.length() / HASH_SIZE);

    /* Parse the hashes */
    metaInfo.piecesHash = (char **) malloc(metaInfo.numPieces * sizeof(char *));
    for (int hashNum = 0; hashNum < metaInfo.numPieces; hashNum++) {
        metaInfo.piecesHash[hashNum] = (char *) malloc(HASH_SIZE * sizeof(char *));
        for (int i = 0; i < HASH_SIZE; ++i) {
            metaInfo.piecesHash[hashNum][i] = hashes[hashNum * HASH_SIZE + i];
        }
    }


    /* Parse the file dictionary */
    bool isInSingleFileMode = !infoDictionary.contains("files");
    if (isInSingleFileMode) {
        metaInfo.directoryName = "";

        /* Parse single file */
        FileInfo info;
        info.numBytes = (int) infoDictionary.getInt("length");
        info.path = infoDictionary.getString("name");
        metaInfo.fileInfos.push_back(info);
        metaInfo.totalSize += info.numBytes;
    } else {
        metaInfo.directoryName = infoDictionary.getString("name");

        /* Parse files */
        const BList &list = infoDictionary.getList("files");
        for (int i = 0; i < list.size(); ++i) {
            const BDictionary &fileInfo = list.getDictionary(i);
            FileInfo info;
            info.numBytes = (int) fileInfo.getInt("length");
            info.path = buildPath(fileInfo);
            metaInfo.fileInfos.push_back(info);
            metaInfo.totalSize += info.numBytes;
        }
    }


}

void Torrent::printHash() const {
    for (int k = 0; k < 20; ++k) {
        char i = metaInfo.infoDictHash[k];
        char map[17] = "0123456789abcdef";
        std::cout << map[i >> 4 & 0xF]
                  << map[i & 0xF];
    }
    std::cout << std::endl;
}

std::string Torrent::buildPath(const BDictionary &pathDictionary) const {
    std::string path;

    const BList &list = pathDictionary.getList("path");
    for (int i = 0; i < list.size(); ++i) {
        const std::string& pathElement = list.getString(i);
        path += pathElement + '/';
    }
    path.pop_back();
    return path;
}
