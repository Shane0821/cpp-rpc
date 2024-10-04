#include "singleton.h"

#include <gtest/gtest.h>

#include <thread>

TEST(SingletonTest, Multithread) {
    class SingletonTest : public Singleton<SingletonTest> {
        friend class Singleton<SingletonTest>;

       public:
        ~SingletonTest() = default;

        int a = 0;
        int b = 0;

       protected:
        SingletonTest() = default;
    };

    std::thread t1([&]() { SingletonTest::GetInst().a++; });

    std::thread t2([&]() { SingletonTest::GetInst().b++; });

    t1.join();
    t2.join();

    EXPECT_EQ(SingletonTest::GetInst().a, 1);
    EXPECT_EQ(SingletonTest::GetInst().b, 1);
}