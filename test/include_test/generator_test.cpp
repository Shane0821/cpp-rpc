#include "generator.h"

#include <gtest/gtest.h>

#include <thread>

TEST(CoroGeneratorTest, Generate) {
    auto gen = []() -> coro::generator<uint64_t> {
        uint64_t i = 0;
        while (true) {
            co_yield i;
            ++i;
        }
    };

    int cnt = 0;
    // Generate the next number until its greater than count to.
    for (auto val : gen()) {
        EXPECT_EQ(val, cnt);
        cnt++;
        if (val >= 10000000) {
            break;
        }
    }
}