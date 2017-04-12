#include "ThreadPool.hpp"

ThreadPool::ThreadPool(int numThreads) : work(nullptr), hasLoopStarted(false) {
    /*
     * This will add 2 threads to the thread pool. (You could just put it in a for loop)
     */
    for (int i = 0; i < numThreads; ++i) {
        threadpool.create_thread(
                boost::bind(&boost::asio::io_service::run, &ioService));
    }
}

/*
 * This will start the ioService processing loop. All tasks
 * assigned with ioService.post() will start executing.
 */
void ThreadPool::start() {
    if (!hasStarted()) {
        work = new boost::asio::io_service::work(ioService);
        hasLoopStarted = true;
    } else {
        throw std::logic_error("The pool has already started!");
    }
}

/*
 * This will stop the ioService processing loop. Any tasks
 * you add behind this point will not execute.
 */
void ThreadPool::stop() {
    if (hasStarted()) {
        ioService.stop();
    } else {
        throw std::logic_error("The pool hasn't started yet!");
    }
}

/*
 * Returns true if the processing loop has started, false otherwise.
 */
bool ThreadPool::hasStarted() {
    return hasLoopStarted;
}


/*
 * Will wait till all the threads in the thread pool are finished with
 * their assigned tasks and 'join' them. Just assume the threads inside
 * the threadpool will be destroyed by this method.
 */
void ThreadPool::joinAll() {
    threadpool.join_all();
}