#include <ThreadPool.hpp>

namespace TT {

    ThreadPool::ThreadPool(const uint32_t threadpoolSize) {
        for(uint32_t num = threadpoolSize; num--;)
            mThreads.emplace_back([this] { taskManagementLoop(); });
    }

    ThreadPool::~ThreadPool() {
        mTerminate = true;
        mThreadPoolConditional.notify_all();
        for(auto& thread : mThreads)
            thread.join();
        mThreads.clear();
    }

    bool ThreadPool::hasTasks () {
        bool empty;
        {
            std::lock_guard<std::mutex> loock(mQueueMutex);
            empty = mTasks.empty();
        }
        return empty;
    }

    void ThreadPool::taskManagementLoop(){
        while(true) {
            std::unique_lock<std::mutex> lock(mQueueMutex);
            mThreadPoolConditional.wait(lock, [this] { return !mTasks.empty() || mTerminate; });
            if(mTerminate) break;
            Task currentTask = std::move(mTasks.front());
            mTasks.pop();
            lock.unlock();
            currentTask();
        }
    }

}