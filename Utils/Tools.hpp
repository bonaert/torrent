#ifndef TORRENT_TOOLS_HPP
#define TORRENT_TOOLS_HPP

#include <string>
#include <vector>

std::string urlencode(const std::string &string);

std::string urldecode(const std::string &string);

bool contains(const std::vector<std::string> &vector, const std::string &element);

#endif //TORRENT_TOOLS_HPP
