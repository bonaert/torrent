#ifndef TORRENT_TOOLS_HPP
#define TORRENT_TOOLS_HPP

#include <string>
#include <vector>

std::string urlencode(const std::string &string);

std::string urldecode(const std::string &string);

bool contains(const std::vector<std::string> &vector, const std::string &element);

bool isDigit(int character);

int readInt(const std::string &string, int pos = 0);

void copyArray(char *source, char *destination, int numChars);

char *writeInt64(char *buffer, int64_t val);

char *writeInt32(char *buffer, int32_t val);

char *writeInt16(char *buffer, int16_t val);

char *writeInt8Array(char *buffer, int8_t *array, int size);

int64_t readInt64AndAdvancePointer(char **bufferPointer);

int32_t readInt32AndAdvancePointer(char **bufferPointer);

int16_t readInt16AndAdvancePointer(char **bufferPointer);

bool startsWith(const std::string &source, const std::string &beginning);

void computeSHA1Hash(unsigned char *result, const std::string &text);

#endif //TORRENT_TOOLS_HPP
