#include "mpmc_queue.h"

#include <gtest/gtest.h>

#include <thread>

struct Item {
   public:
    Item() = default;
    Item(int a, int b) : a(a), b(b) {}

    int a = 0;
    int b = 0;
};

TEST(MPMCQueueTest, Push) {
    MPMCQueue<Item, 1024> q;
    for (int i = 0; i < 1023; ++i) {
        ASSERT_TRUE(q.emplace(i, i + 1));
    }
    ASSERT_FALSE(q.emplace(1023, 1024));
}

TEST(MPMCQueueTest, Pop) {
    MPMCQueue<Item, 1024> q;
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
}

TEST(MPMCQueueTest, Size) {
    MPMCQueue<Item, 1024> q;
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

TEST(MPMCQueueTest, Empty) {
    MPMCQueue<Item, 1024> q;
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

TEST(MPMCQueueTest, SPMC) {
    constexpr int capacity = 100000;

    std::vector<int> vis(capacity, 0);

    MPMCQueue<Item, capacity> q;
    std::thread producer1([&q] {
        for (int i = 0; i < capacity - 1; ++i) {
            ASSERT_TRUE(q.emplace(i, i + 1));
        }
    });

    std::thread consumer1([&] {
        for (int i = 0; i < capacity / 4; ++i) {
            Item item;
            while (!q.pop(item));
            ASSERT_EQ(vis[item.a], 0);
            vis[item.a] = 1;
        }
    });
    std::thread consumer2([&] {
        for (int i = 0; i < capacity / 4; ++i) {
            Item item;
            while (!q.pop(item));
            ASSERT_EQ(vis[item.a], 0);
            vis[item.a] = 1;
        }
    });
    std::thread consumer3([&] {
        for (int i = 0; i < capacity / 2 - 1; ++i) {
            Item item;
            while (!q.pop(item));
            ASSERT_EQ(vis[item.a], 0);
            vis[item.a] = 1;
        }
    });

    producer1.join();
    consumer1.join();
    consumer2.join();
    consumer3.join();

    ASSERT_EQ(q.size(), 0);
    ASSERT_EQ(q.empty(), true);
    for (int i = 0; i < capacity - 1; ++i) {
        ASSERT_TRUE(vis[i]);
    }
}

TEST(MPMCQueueTest, MPMC) {
    constexpr int capacity = 100000;

    std::vector<int> vis(capacity, 0);

    MPMCQueue<Item, capacity> q;
    std::thread producer1([&q] {
        for (int i = 0; i < capacity / 4; ++i) {
            ASSERT_TRUE(q.emplace(i, i + 1));
        }
    });
    std::thread producer2([&q] {
        for (int i = capacity / 4; i < capacity / 2; ++i) {
            ASSERT_TRUE(q.emplace(i, i + 1));
        }
    });
    std::thread producer3([&q] {
        for (int i = capacity / 2; i < capacity - 1; ++i) {
            ASSERT_TRUE(q.emplace(i, i + 1));
        }
    });

    std::thread consumer1([&] {
        for (int i = 0; i < capacity / 4; ++i) {
            Item item;
            while (!q.pop(item));
            ASSERT_EQ(vis[item.a], 0);
            vis[item.a] = 1;
        }
    });
    std::thread consumer2([&] {
        for (int i = 0; i < capacity / 4; ++i) {
            Item item;
            while (!q.pop(item));
            ASSERT_EQ(vis[item.a], 0);
            vis[item.a] = 1;
        }
    });
    std::thread consumer3([&] {
        for (int i = 0; i < capacity / 2 - 1; ++i) {
            Item item;
            while (!q.pop(item));
            ASSERT_EQ(vis[item.a], 0);
            vis[item.a] = 1;
        }
    });

    producer1.join();
    producer2.join();
    producer3.join();
    consumer1.join();
    consumer2.join();
    consumer3.join();

    ASSERT_EQ(q.size(), 0);
    ASSERT_EQ(q.empty(), true);
    for (int i = 0; i < capacity - 1; ++i) {
        ASSERT_TRUE(vis[i]);
    }
}