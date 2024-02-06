#include <ThreadPool.hpp>

using TT::ThreadPool;


int baseTest(int argc, char** argv) {
    uint32_t error = 0;

    // create a threadpool with the maximum amount of threads
    ThreadPool tp;

    // at this point there should not be any tasks
    if (tp.hasTasks())
        error += 1;

    if(tp.idleThreads() != tp.maxThreads())
        error += 1<<1;

    try {
        tp.pause();

        if(!tp.isPaused())
            error += 1<<2;

        tp.resume();
        if(tp.isPaused())
            error += 1<<3;
    } catch (...) {
        error += 1<<4;
    }

    try {
        auto future = tp.queueJob([]{
            return true;
        });

        if(!future.get()) error += 1<<5;
    } catch(...) {
        error += 1 << 6;
    }

    return error;
}