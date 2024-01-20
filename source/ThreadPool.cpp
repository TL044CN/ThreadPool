#include <ThreadPool.hpp>

namespace TT {

ThreadPool::ThreadPool(const uint32_t poolSize)
    : mPoolSize(poolSize) {
    for ( auto index = mPoolSize; index--;)
        mThreads.emplace_back([this] { runTasks(); });
}

ThreadPool::~ThreadPool() {
    {
        uLock lock(mQueueMutex);
        mTerminate = true;
    }
    mConditionVariable.notify_all();
    for ( std::thread& thread : mThreads )
        thread.join();

    mThreads.clear();
}

bool ThreadPool::isBusy() {
    bool empty;
    {
        uLock lock(mQueueMutex);
        empty = mTaskQueue.empty();
    }
    return empty;
}

std::future<void> ThreadPool::getWaitFuture() {
    return std::async([this]{
        while(true){
            uLock lock(mQueueMutex);
            if(mTaskQueue.empty()) return;
        }
    });
}

void ThreadPool::wait(){
    getWaitFuture().wait();
}

void ThreadPool::runTasks() {
    while ( true ) {
        std::packaged_task<void()> task;
        {
            uLock lock(mQueueMutex);
            mConditionVariable.wait(lock, [this] { return mTaskQueue.empty() || mTerminate; });
            if ( mTerminate ) return;

            task = std::move(mTaskQueue.front());
            mTaskQueue.pop();
        }
        task();
    }
}

}