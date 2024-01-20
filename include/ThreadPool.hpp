#ifndef TT_THREADPOOL_HPP
#define TT_THREADPOOL_HPP

#include <queue>
#include <future>
#include <mutex>
#include <vector>
#include <thread>
#include <condition_variable>
#include <cstdint>

namespace TT {

/**
 * @brief Manages threads for executing multiple functions in parallel, while using
 * only a certain number of threads.
 */
class ThreadPool {
    using Task = std::packaged_task<void()>;
    using uLock = std::unique_lock<std::mutex>;
private:
    std::queue<Task> mTaskQueue;
    std::vector<std::thread> mThreads;

    std::mutex mQueueMutex;
    std::condition_variable mConditionVariable;

    bool mTerminate = false;
    const uint32_t mPoolSize;

public:
    /**
     * @brief Construct a new Thread Pool and populate threads
     *
     */
    ThreadPool(const uint32_t poolSize = std::thread::hardware_concurrency());

    /**
     * @brief Destroy the Thread Pool and stop operation
     *
     */
    ~ThreadPool();

public:
    /**
     * @brief Enqueues a new Task
     *
     * @tparam Func the type of the Function to run
     * @tparam Ret the return type of the Function
     * @param function the Function to run
     * @return std::future<Ret> Future with the return value of the function
     */
    template<typename Func, typename Ret = std::result_of_t<Func && ()>>
    std::future<Ret> queueJob(Func&& function) {
        std::packaged_task<Ret()> task = std::packaged_task<Ret()>(std::forward<Func>(function));
        std::future<Ret> future = task.get_future();
        {
            std::lock_guard<std::mutex> lock(mQueueMutex);
            mTaskQueue.emplace(Task(std::move(task)));
        }
        mConditionVariable.notify_one();
        return future;
    }

    /**
     * @brief Returns weather the Threadpool is busy or not.
     *
     * @return true Threadpool is currently processing
     * @return false Threadpool is idle
     */
    bool isBusy();

    /**
     * @brief Returns a Future to wait for, which resolves once no more Tasks are to be run
     * @return std::future<void> future for when all tasks are done.
     */
    std::future<void> getWaitFuture();

    /**
     * @brief Waits until no more tasks are to be run
     */
    void wait();

private:
    /**
     * @brief Main run function of the executing threads
     *
     */
    void runTasks();

};

}

#endif