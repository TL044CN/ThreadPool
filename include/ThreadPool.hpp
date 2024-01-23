/**
 * @file ThreadPool.hpp
 * @author TL044CN
 * @brief Threadpool class for multithreading purposes
 * @version 0.2
 * @date 2024-01-21
 *
 * @copyright Copyright (c) TL044CN 2024
 *
 */
#ifndef TT_THREADPOOL_HPP
#define TT_THREADPOOL_HPP

#include <vector>
#include <thread>
#include <queue>
#include <future>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <cstdint>

namespace TT {

/**
 * @brief Manages a number of Threads to do Tasks in parallel (using true multithreading)
 */
class ThreadPool {
    using Task = std::packaged_task<void()>;

    template<typename Func>
#if __cpp_lib_is_invocable
    using resultType = std::invoke_result<Func && ()>::type;
#else
    using resultType = std::result_of<Func && ()>::type;
#endif

private:
    std::vector<std::thread> mThreads;
    std::queue<std::packaged_task<void()>> mTasks;

    std::mutex mQueueMutex;
    std::condition_variable mThreadPoolConditional;

    std::atomic<bool> mTerminate = false;
    std::atomic<uint32_t> mIdleThreads;
    std::atomic_flag mPause;

public:
    /**
     * @brief Create and start new Thread Pool with the given amount of Threads
     *
     * @param threadpoolSize number of Threads, defaults to the number of Hardware threads
     */
    ThreadPool(const uint32_t threadpoolSize = std::thread::hardware_concurrency());

    /**
     * @brief Cleans up the Threads of the Threadpool
     */
    ~ThreadPool();

public:
    /**
     * @brief Add a new Task to the Thread Pool
     *
     * @tparam Func Type of the Function to queue
     * @tparam std::result_of_t<Func &&()> Result type of the Function to queue
     * @param function the Function to queue
     * @return std::future<Ret> Future, holding the returned value of the Function
     */
    template <typename Func, typename Ret = resultType<Func>>
    std::future<Ret> queueJob(Func&& function) {
        auto task = std::packaged_task<Ret()>(std::forward<Func>(function));
        auto future = task.get_future();
        {
            std::lock_guard<std::mutex> lock(mQueueMutex);
            mTasks.emplace(Task(std::move(task)));
        }
        mThreadPoolConditional.notify_one();
        return future;
    }

    /**
     * @brief Pauses the execution of new Tasks
     */
    void pause();

    /**
     * @brief Resumes the execution of new Tasks
     */
    void resume();

    /**
     * @brief Returns the number of idle Threads
     *
     * @return uint32_t the number of idle threads
     */
    uint32_t idleThreads() const;

    /**
     * @brief Returns true when the ThreadPool has active tasks
     *
     * @return true the Threadpool has active tasks
     * @return false the Threadpool is idle
     */
    bool hasTasks();

private:
    /**
     * @brief Handles the execution of Tasks
     */
    void taskManagementLoop();

};

}

#endif