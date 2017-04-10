#ifndef TORRENT_THREADPOOL_HPP
#define TORRENT_THREADPOOL_HPP

#include <boost/asio/io_service.hpp>
#include <boost/thread/thread.hpp>


class ThreadPool {
    /*
     * Create an asio::io_service and a thread_group (through pool in essence)
     */
    boost::asio::io_service ioService;
    boost::thread_group threadpool;
    boost::asio::io_service::work *work;
    bool hasLoopStarted;

public:
    ThreadPool(int numThreads);

    bool hasStarted();

    void start();

    void stop();

    void joinAll();


    template<typename F>
    void postTask(F f);

    template<typename F, typename A>
    void postTask(F f, A a);
};


#endif //TORRENT_THREADPOOL_HPP
