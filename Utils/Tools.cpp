#include "Tools.hpp"

#include <algorithm>
#include <iomanip>
#include <sstream>

std::string urlencode(const std::string &string) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (char c : string) {
        // Keep alphanumeric and other accepted characters intact
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
            continue;
        }

        // Any other characters are percent-encoded
        escaped << std::uppercase;
        escaped << '%' << std::setw(2) << int((unsigned char) c);
        escaped << std::nouppercase;
    }

    const std::string &result = escaped.str();
    return result;
}

std::string urldecode(const std::string &string) {
    std::string decoded = "";
    int i = 0;

    while (i < string.length()) {
        if (string[i] == '%') {
            char buffer[3] = {string[i + 1], string[i + 2], 0};
            long character = strtol(buffer, NULL, 16);
            decoded.push_back((char) character);
            i += 3;
        } else {
            decoded.push_back(string[i]);
            i++;
        }
    }

    return decoded;
}

bool contains(const std::vector<std::string> &vector, const std::string &element) {
    return std::find(vector.begin(), vector.end(), element) != vector.end();
}

bool isDigit(int character) {
    return '0' <= character && character <= '9';
}

int readInt(const std::string &string, int pos) {
    int res = 0;
    while (pos < string.size() && isDigit(string[pos])) {
        int character = string[pos];
        res *= 10;
        res += (character - '0');
        pos++;
    }
    return res;
}


void copyArray(char *source, char *destination, int numChars) {
    for (int i = 0; i < numChars; ++i) {
        destination[i] = source[i];
    }
}

char *writeInt64(char *buffer, int64_t val) {
    *((uint64_t *) buffer) = htobe64((uint64_t) val);
    return buffer + 8;
}

char *writeInt32(char *buffer, int32_t val) {
    *((uint32_t *) buffer) = htobe32((uint32_t) val);
    return buffer + 4;
}

char *writeInt16(char *buffer, int16_t val) {
    *((uint16_t *) buffer) = htobe16((uint32_t) val);
    return buffer + 2;
}

char *writeInt8Array(char *buffer, int8_t *array, int size) {
    for (int i = 0; i < size; ++i) {
        buffer[i] = array[i]; // No need to convert to big endian since it's only 1 byte
    }
    return buffer + size;
}


int64_t readInt64AndAdvancePointer(char **bufferPointer) {
    char *buffer = *bufferPointer;
    uint64_t value = *((uint64_t *) buffer);
    *bufferPointer += 8;
    return (int64_t) be64toh(value);
}

int32_t readInt32AndAdvancePointer(char **bufferPointer) {
    char *buffer = *bufferPointer;
    uint32_t value = *((uint32_t *) buffer);
    *bufferPointer += 4;
    return (int32_t) be32toh(value);
}

int16_t readInt16AndAdvancePointer(char **bufferPointer) {
    char *buffer = *bufferPointer;
    uint16_t value = *((uint16_t *) buffer);
    *bufferPointer += 2;
    return (int16_t) be16toh(value);
}

bool startsWith(const std::string &source, const std::string &beginning) {
    return source.compare(0, beginning.size(), beginning) == 0;
}