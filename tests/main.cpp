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
        }

        while(tp.isBusy()){}
        
        for ( auto& future : futures )
            std::cout << "i: " << future.get() << '\n';

    } catch ( std::exception e ) {
        std::cout << "Exception: " << e.what() << "\n";
    }

    return EXIT_SUCCESS;
}
