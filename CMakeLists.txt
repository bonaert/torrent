cmake_minimum_required(VERSION 3.7)
project(torrent)

set(CMAKE_CXX_STANDARD 11)


set(SOURCE_FILES
        Torrent.cpp Torrent.hpp
        Client.cpp Client.hpp
        Peer/PeerManager.cpp Peer/PeerManager.hpp
        Peer/PeerConnection.cpp Peer/PeerConnection.hpp
        BEncode.cpp BEncode.hpp

        Tracker/TrackerMaster.cpp Tracker/TrackerMaster.hpp
        Tracker/Tracker.cpp Tracker/Tracker.hpp
        Tracker/UDPTracker.cpp Tracker/UDPTracker.hpp
        Tracker/HTTPTracker.cpp Tracker/HTTPTracker.hpp

        Utils/Tools.cpp Utils/Tools.hpp
        Utils/UDPCommunicator.cpp Utils/UDPCommunicator.hpp
        Utils/Networking.cpp Utils/Networking.hpp
        Utils/ThreadPool.cpp Utils/ThreadPool.hpp
        )

add_executable(torrent main.cpp ${SOURCE_FILES})
add_executable(torrentTest test.cpp ${SOURCE_FILES})


set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS thread system)
find_package(Threads REQUIRED)

if (Boost_FOUND)
    target_link_libraries(torrent ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
    target_include_directories(torrent PUBLIC SYSTEM ${Boost_INCLUDE_DIRS})

    target_link_libraries(torrentTest ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
    target_include_directories(torrentTest PUBLIC SYSTEM ${Boost_INCLUDE_DIRS})
endif ()

target_link_libraries(torrent Threads::Threads)
target_link_libraries(torrent curl)
target_link_libraries(torrent crypto)

target_link_libraries(torrentTest Threads::Threads)
target_link_libraries(torrentTest curl)
target_link_libraries(torrentTest crypto)
