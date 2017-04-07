#ifndef TORRENT_BENCODE_HPP
#define TORRENT_BENCODE_HPP

#include <string>
#include <fstream>
#include <map>
#include <vector>

using intType = int64_t;

// Root class
class BItem {};
class BInteger;
class BString;
class BList;
class BDictionary;

BItem *parseBItem(std::istream &stream);


// Integer
class BInteger : public BItem {
private:
    intType value;
public:
    BInteger(int number) : value(number) {};

    BInteger(std::istream &stream);

    operator intType() { return value; } // Conversion to underlying value
};


// String
class BString : public BItem {
private:
    std::string string;
public:
    BString(const std::string string) : string(string) {};

    BString(std::istream &stream);

    operator std::string &() { return string; }
};


// String
class BList : public BItem {
private:
    std::vector<BItem *> items;
public:
    BList(std::istream &stream);

    std::string getString(int index) const;

    intType getInt(int index) const;

    BList &getList(int index);

    BDictionary & getDictionary(int index)const;

    int size()const;
};


// Dictionnary
class BDictionary : public BItem {
    std::map<std::string, BItem *> itemMap;
public:
    BDictionary(std::istream &stream);

    bool contains(const std::string &key) const;

    std::string getString(const std::string &key) const;

    intType getInt(const std::string &key) const;

    BList & getList(const std::string &key)const;

    BDictionary &getDictionary(const std::string &key);

    int size();
};


/*
 * Helper functions
 */

int readInt(std::istream &stream);


#endif //TORRENT_BENCODE_HPP
