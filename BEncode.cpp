#include <assert.h>
#include "BEncode.hpp"


/*
 * INTEGER
 */

BInteger::BInteger(std::ifstream &file) {
    assert(file.get() == 'i');

    bool isNegative = false;
    if (file.peek() == '-') {
        isNegative = true;
        file.get();
    }

    value = 0;
    while (file.peek() != 'e') {
        int character = file.get();
        value *= 10;
        value += (character - '0');
    }

    assert(file.get() == 'e');
    if (isNegative) {
        value = -value;
    }

}




/*
 * STRING
 */

BString::BString(std::ifstream &file) {
    int length = readInt(file); // Reads the size
    int i = file.get();
    assert(i == ':');  // Reads the separator


    char buffer[length];
    file.read(buffer, length);
    string = std::string(buffer, (unsigned long) length);
}



/*
 * LIST
 */

BList::BList(std::ifstream &stream) {
    assert(stream.get() == 'l');

    while (stream.peek() != 'e'){
        BItem * item = parseBItem(stream);
        items.push_back(item);
    }

    assert(stream.get() == 'e');
}


intType BList::getInt(int index) const {
    BItem *type = items[index];
    BInteger *integer = static_cast<BInteger *>(type);
    return *integer; // Implicit conversion from BInteger to intType
}

std::string BList::getString(int index) const {
    BItem *type = items[index];
    BString * string = static_cast<BString *>(type);
    return *string; // Implicit conversion from BString to std::string
}

BList &BList::getList(int index) {
    BItem *type = items[index];
    BList * list = static_cast<BList *>(type);
    return *list;
}

BDictionary & BList::getDictionary(int index)const {
    BItem *type = items[index];
    BDictionary * dictionary = static_cast<BDictionary *>(type);
    return *dictionary;
}

int BList::size()const {
    return (int) items.size();
}


/*
 * DICTIONARY
 */

BDictionary::BDictionary(std::ifstream& stream) {
    assert(stream.get() == 'd');

    while (stream.peek() != 'e'){
        std::string key = BString(stream);
        BItem * item = parseBItem(stream);
        itemMap[key] = item;
    }

    assert(stream.get() == 'e');
}


bool BDictionary::contains(const std::string &key) const {
    return itemMap.find(key) != itemMap.end();
}

intType BDictionary::getInt(const std::string &key) const {
    BItem *type = itemMap.at(key);
    BInteger *integer = static_cast<BInteger *>(type);
    return *integer; // Implicit conversion from BInteger to intType
}

std::string BDictionary::getString(const std::string &key) const {
    BItem *type = itemMap.at(key);
    BString * string = static_cast<BString *>(type);
    return *string; // Implicit conversion from BString to std::string
}

BList & BDictionary::getList(const std::string &key)const {
    BItem *type = itemMap.at(key);
    BList * list = static_cast<BList *>(type);
    return *list;
}

BDictionary & BDictionary::getDictionary(const std::string &key) {
    BItem *type = itemMap.at(key);
    BDictionary * dictionary = static_cast<BDictionary *>(type);
    return *dictionary;
}

int BDictionary::size() {
    return (int) itemMap.size();
}



/*
 * Parsing functions
 */

BItem *parseBItem(std::ifstream &file) {
    if (file.peek() == 'i') {
        return new BInteger(file);
    } else if (file.peek() == 'd') {
        return new BDictionary(file);
    } else if (file.peek() == 'l') {
        return new BList(file);
    } else {
        return new BString(file);
    }
}


bool isDigit(int character) {
    return '0' <= character && character <= '9';
}

int readInt(std::ifstream& file) {
    int res = 0;
    while (isDigit(file.peek())) {
        int character = file.get();
        res *= 10;
        res += (character - '0');
    }
    return res;
}
