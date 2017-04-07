cmake_minimum_required(VERSION 3.7)
project(torrent)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        Torrent.cpp Torrent.hpp
        Tracker.cpp Tracker.hpp
        Client.cpp Client.hpp
        Peer.cpp Peer.hpp


        BEncode.cpp BEncode.hpp

        Utils/Tools.cpp Utils/Tools.hpp
        Utils/UDPCommunicator.cpp Utils/UDPCommunicator.hpp
        Utils/Networking.cpp Utils/Networking.hpp
        )

add_executable(torrent main.cpp ${SOURCE_FILES} TrackerMaster.cpp TrackerMaster.hpp)
add_executable(torrentTest test.cpp ${SOURCE_FILES})

target_link_libraries(torrent curl)
target_link_libraries(torrentTest curl)
