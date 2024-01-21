#include <iostream>
#include <ThreadPool.hpp>

#include <vector>
#include <future>

using namespace TT;

int main() {
    std::cout << "Mighty Debugging Tool\n";
    try {
        ThreadPool tp;
        std::vector<std::future<int>> futures;

        try {
            for ( uint32_t i = 1000; i--;)
                futures.emplace_back(tp.queueJob([i]() -> int { return i; }));

        } catch ( std::exception e ) {
            std::cout << "Exception: " << e.what() << "\n";
            return EXIT_FAILURE;
        }

        for ( auto& future : futures )
            std::cout << "i: " << future.get() << '\n';

        try {
            std::condition_variable cv;
            std::mutex mutex;

            mutex.lock();
            auto future = tp.queueJob([&] {
                std::unique_lock<std::mutex> lock(mutex);
                cv.wait(lock, [] {return true;});
                return 256;
            });

            mutex.unlock();
            cv.notify_one();

            if ( future.get() != 256 )
                return EXIT_FAILURE;

        } catch ( std::exception e ) {
            std::cout << "Exception: " << e.what() << "\n";
            return EXIT_FAILURE;
        }

    } catch ( std::exception e ) {
        std::cout << "Exception: " << e.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
