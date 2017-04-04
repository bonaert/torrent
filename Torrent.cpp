#include "Torrent.hpp"
#include <fstream>

Torrent::Torrent(std::string torrentFileName) : torrentFileName(torrentFileName) {
    parseTorrentFile();
}

int Torrent::size() {
    return 0;
}

int Torrent::numPieces() {
    return 0;
}

int Torrent::pieceSize() {
    return 0;
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
    return "";
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
    metaInfo.announceURL = metaInfoDict.getString("annouce");

    if (metaInfoDict.contains("creation date")) {
        metaInfo.creationDate = metaInfoDict.getInt("creation date");
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

    metaInfo.infoDictHash = hash(infoDictionary.getUnderlyingString());
    metaInfo.pieceLength = infoDictionary.getInt("piece length");
    if (infoDictionary.contains("private")) {
        metaInfo.isPrivate = (infoDictionary.getInt("private") == 1);
    }

    const std::string &hashes = infoDictionary.getString("pieces");
    metaInfo.numPieces = (int) (hashes.length() / HASH_SIZE);

    /* Parse the hashes */
    metaInfo.piecesHash = (char[metaInfo.numPieces][HASH_SIZE]) malloc(metaInfo.numPieces * sizeof(char[HASH_SIZE]));
    for (int hashNum = 0; hashNum < metaInfo.numPieces; hashNum++) {
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
        info.numBytes = infoDictionary.getInt("length");
        info.path = infoDictionary.getString("name");
        metaInfo.fileInfos.push_back(info);
    } else {
        metaInfo.directoryName = infoDictionary.getString("name");

        /* Parse files */
        for (BencodedDictionary &fileInfo: infoDictionary.getDictionaryList("files")) {
            FileInfo info;
            info.numBytes = fileInfo.getInt("length");
            info.path = buildPath(infoDictionary);
            metaInfo.fileInfos.push_back(info);
        }
    }


}

std::string Torrent::buildPath(const BDictionary &pathDictionary) const {
    std::string path;

    for (std::string &pathElement: pathDictionary.getStringList("path")) {
        path += pathElement + '/';
    }
    path.pop_back();
    return path;
}
