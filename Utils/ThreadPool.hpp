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


    /*
     * If we don't put the definition here, there are compilation errors
     * https://stackoverflow.com/questions/10632251/undefined-reference-to-template-function
     * https://stackoverflow.com/questions/495021/why-can-templates-only-be-implemented-in-the-header-file
     *
     * "The implementation of a non-specialized template must be visible to a translation
     * unit that uses it. The compiler must be able to see the implementation in order
     * to generate code for all specializations in your code."
     */

    /*
     * This will assign tasks to the thread pool.
     * More about boost::bind: "http://www.boost.org/doc/libs/1_54_0/libs/bind/bind.html#with_functions"
     */
    template<typename F>
    void postTask(F f) {
        ioService.post(boost::bind(f));
    };

    /*
     * This will assign tasks to the thread pool.
     * More about boost::bind: "http://www.boost.org/doc/libs/1_54_0/libs/bind/bind.html#with_functions"
     */
    template<typename F, typename A>
    void postTask(F f, A a) {
        ioService.post(boost::bind(f, a));
    };
};


#endif //TORRENT_THREADPOOL_HPP
