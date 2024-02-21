#include <ThreadPool.hpp>
#include <cstdint>
#include <ranges>
#include <iostream>
#include <functional>

constexpr uint32_t c_min_value = 1;
constexpr uint32_t c_max_value = 100000;

std::string runFizzBuzz(uint32_t min, uint32_t max) {
    std::string result;
    for ( auto num : std::views::iota(min, max) ) {
        std::string append;
        if ( ! (num % 3) ) append += "Fizz";
        if ( ! (num % 5) ) append += "Buzz";
        if ( append.size() == 0 ) append += std::to_string(num);
        result += append + " ";
    }
    return result;
}

int FizzBuzz(int argc, char** argv) {

    TT::ThreadPool pool;
    std::vector<std::future<std::string>> mFutures;

    const uint32_t stride = c_max_value / pool.maxThreads();

    for ( uint32_t min = c_min_value; min < c_max_value; min += stride ) {
        uint32_t max = min + stride;
        
        if ( max > c_max_value )
            max = c_max_value;
        
        mFutures.push_back(pool.queueJob(std::bind(runFizzBuzz, min, max)));
    }

    std::cout << "Result:\n";
    for ( auto& future : mFutures )
        std::cout << future.get();
    std::cout << '\n';

    return EXIT_SUCCESS;
}