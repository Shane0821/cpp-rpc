#include "spsc_queue.h"

#include <gtest/gtest.h>

#include <thread>

struct Item {
   public:
    Item() = default;
    Item(int a, int b) : a(a), b(b) {}

    int a = 0;
    int b = 0;
};

TEST(SPSCQueueTest, Push) {
    SPSCQueue<Item, 1024> q;
    for (int i = 0; i < 1023; ++i) {
        ASSERT_TRUE(q.emplace(i, i + 1));
    }
    ASSERT_FALSE(q.emplace(1023, 1024));
}

TEST(SPSCQueueTest, Pop) {
    SPSCQueue<Item, 1024> q;
    for (int i = 0; i < 1023; ++i) {
        q.emplace(i, i + 1);
    }
    for (int i = 0; i < 1023; ++i) {
        Item item;
        ASSERT_TRUE(q.pop(item));
        ASSERT_EQ(item.a, i);
        ASSERT_EQ(item.b, i + 1);
    }
    Item item;
    ASSERT_FALSE(q.pop(item));

    for (int i = 0; i < 1023; ++i) {
        q.emplace(i, i + 1);
    }
    for (int i = 0; i < 1023; ++i) {
        Item item;
        ASSERT_TRUE(q.pop(item));
        ASSERT_EQ(item.a, i);
        ASSERT_EQ(item.b, i + 1);
    }
    ASSERT_FALSE(q.pop(item));
}

TEST(SPSCQueueTest, Size) {
    SPSCQueue<Item, 1024> q;
    for (int i = 0; i < 1023; ++i) {
        ASSERT_TRUE(q.emplace(i, i + 1));
        ASSERT_EQ(q.size(), i + 1);
    }
    ASSERT_FALSE(q.emplace(1023, 1024));
    ASSERT_EQ(q.size(), 1023);

    for (int i = 0; i < 1023; ++i) {
        Item item;
        ASSERT_TRUE(q.pop(item));
        ASSERT_EQ(q.size(), 1023 - i - 1);
    }
}

TEST(SPSCQueueTest, Empty) {
    SPSCQueue<Item, 1024> q;
    ASSERT_EQ(q.empty(), true);
    for (int i = 0; i < 1023; ++i) {
        q.emplace(i, i + 1);
        ASSERT_EQ(q.empty(), false);
    }

    for (int i = 0; i < 1023; ++i) {
        ASSERT_EQ(q.empty(), false);
        Item item;
        q.pop(item);
    }
    ASSERT_EQ(q.empty(), true);
}

TEST(SPSCQueueTest, Concurrency) {
    constexpr int capacity = 10000000;
    SPSCQueue<Item, capacity> q;
    std::thread producer([&q] {
        for (int i = 0; i < capacity - 1; ++i) {
            ASSERT_TRUE(q.emplace(i, i + 1));
        }
    });

    std::thread consumer([&q] {
        for (int i = 0; i < capacity - 1; ++i) {
            Item item;
            while (!q.pop(item)) {
            }
            ASSERT_EQ(item.a, i);
            ASSERT_EQ(item.b, i + 1);
        }
        ASSERT_EQ(q.empty(), true);
    });

    producer.join();
    consumer.join();
}