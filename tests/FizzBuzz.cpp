#include <ThreadPool.hpp>
#include <cstdint>
#include <ranges>
#include <iostream>
#include <functional>

constexpr uint32_t c_min_value = 1;
constexpr uint32_t c_max_value = 100000000;

std::string runFizzBuzz(std::ranges::input_range auto&& nums) {
    std::string result;
    for ( const auto& num : nums ) {
        if ( ! (num % 3) ) result += "Fizz";
        if ( ! (num % 5) ) result += "Buzz";
        if ( ! (num % 3 || num % 5) ) result += std::to_string(num);
        result += " ";
    }
    return result;
}

int FizzBuzz(int argc, char** argv) {

    TT::ThreadPool pool;
    std::vector<std::future<std::string>> mFutures;

    const uint32_t stride = c_max_value / pool.maxThreads();

    for ( auto& chunk : std::views::iota(c_min_value, c_max_value) | std::views::chunk(3) )
        mFutures.push_back(pool.queueJob(std::bind(runFizzBuzz, chunk)));

    std::cout << "Result:\n";
    for ( auto& future : mFutures )
        std::cout << future.get();
    std::cout << '\n';

    return EXIT_SUCCESS;
}