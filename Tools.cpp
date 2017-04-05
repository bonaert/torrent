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