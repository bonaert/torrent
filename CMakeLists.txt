cmake_minimum_required(VERSION 3.7)
project(torrent)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp Torrent.cpp Torrent.hpp Tracker.cpp Tracker.hpp)
add_executable(torrent ${SOURCE_FILES})