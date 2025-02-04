#include "thread_pool.h"

#include <gtest/gtest.h>

#include <future>

TEST(ThreadPoolTest, FIFOThreadPool) {
    ThreadPool<FIFOScheduler> pool(4);
    std::vector<std::future<std::string>> results;

    // start eight thread task
    for (int i = 0; i < 8; ++i) {
        // add all task to result list
        results.emplace_back(
            // add print task to thread pool
            pool.add([i] {
                std::cout << "hello " << i << std::endl;
                // wait a sec when the previous line is out
                std::this_thread::sleep_for(std::chrono::seconds(1));
                // keep output and return the status of execution
                std::cout << "world " << i << std::endl;
                return std::string("---thread ") + std::to_string(i) +
                       std::string(" finished.---");
            }));
    }
    for (int i = 0; i < 8; ++i) {
        // check the status of each thread
        EXPECT_EQ(results[i].get(), std::string("---thread ") + std::to_string(i) +
                                        std::string(" finished.---"));
    }
}

TEST(ThreadPoolTest, LIFOThreadPool) {
    ThreadPool<LIFOScheduler> pool(4);
    std::vector<std::future<std::string>> results;

    // start eight thread task
    for (int i = 0; i < 8; ++i) {
        // add all task to result list
        results.emplace_back(
            // add print task to thread pool
            pool.add([i] {
                std::cout << "hello " << i << std::endl;
                // wait a sec when the previous line is out
                std::this_thread::sleep_for(std::chrono::seconds(1));
                // keep output and return the status of execution
                std::cout << "world " << i << std::endl;
                return std::string("---thread ") + std::to_string(i) +
                       std::string(" finished.---");
            }));
    }
    for (int i = 7; i >= 0; --i) {
        // check the status of each thread
        EXPECT_EQ(results[i].get(), std::string("---thread ") + std::to_string(i) +
                                        std::string(" finished.---"));
    }
}

TEST(ThreadPoolTest, TimerEventScheduler) {
    ThreadPool<FIFOScheduler> pool(4);
    std::vector<std::future<std::string>> results;

    // start eight thread task
    for (int i = 0; i < 8; ++i) {
        // add all task to result list
        results.emplace_back(
            // add print task to thread pool
            pool.add([i] {
                std::cout << "hello " << i << std::endl;
                // wait a sec when the previous line is out
                std::this_thread::sleep_for(std::chrono::seconds(1));
                // keep output and return the status of execution
                std::cout << "world " << i << std::endl;
                return std::string("---thread ") + std::to_string(i) +
                       std::string(" finished.---");
            }));
    }
    for (int i = 8; i < 12; ++i) {
        // add all task to result list
        results.emplace_back(
            // add print task to thread pool
            pool.add_delayed(std::chrono::seconds(i),
                             [i] {
                                 std::cout << "hello " << i << std::endl;
                                 return std::string("---thread ") + std::to_string(i) +
                                        std::string(" finished.---");
                             })
                .second);
    }
    for (int i = 12; i < 16; ++i) {
        // add all task to result list
        results.emplace_back(
            // add print task to thread pool
            pool.add_delayed(std::chrono::seconds(i),
                             [i] {
                                 std::cout << "hello " << i << std::endl;
                                 return std::string("---thread ") + std::to_string(i) +
                                        std::string(" finished.---");
                             })
                .second);
    }
    // calcel last 4
    for (int i = 4; i < 8; i++) {
        pool.cancel_delayed(i);
    }
    for (int i = 0; i < 8; ++i) {
        // check the status of each thread
        EXPECT_EQ(results[i].get(), std::string("---thread ") + std::to_string(i) +
                                        std::string(" finished.---"));
    }
    for (int i = 8; i < 12; ++i) {
        // check the status of each thread
        EXPECT_EQ(results[i].get(), std::string("---thread ") + std::to_string(i) +
                                        std::string(" finished.---"));
    }
    for (int i = 12; i < 16; ++i) {
        // check the status of each thread
        // std::future_error: Broken promise
        EXPECT_THROW(results[i].get(), std::future_error);
    }
}