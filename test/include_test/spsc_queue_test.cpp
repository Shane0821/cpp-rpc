#include "spsc_queue.h"

#include <gtest/gtest.h>

struct Item {
   public:
    Item() = default;
    Item(int a, int b) : a(a), b(b) {}

    int a = 0;
    int b = 0;
};

TEST(SPSCQueueTest, PushTest) {
    SPSCQueue<Item, 1024> q;
    for (int i = 0; i < 1023; ++i) {
        EXPECT_TRUE(q.emplace(i, i + 1));
    }
    EXPECT_FALSE(q.emplace(1023, 1024));
}

TEST(SPSCQueueTest, PopTest) {
    SPSCQueue<Item, 1024> q;
    for (int i = 0; i < 1023; ++i) {
        q.emplace(i, i + 1);
    }
    for (int i = 0; i < 1023; ++i) {
        Item item;
        EXPECT_TRUE(q.pop(item));
        EXPECT_EQ(item.a, i);
        EXPECT_EQ(item.b, i + 1);
    }
    Item item;
    EXPECT_FALSE(q.pop(item));
}

TEST(SPSCQueueTest, SizeTest) {
    SPSCQueue<Item, 1024> q;
    for (int i = 0; i < 1023; ++i) {
        EXPECT_TRUE(q.emplace(i, i + 1));
        EXPECT_EQ(q.size(), i + 1);
    }
    EXPECT_FALSE(q.emplace(1023, 1024));
    EXPECT_EQ(q.size(), 1023);

    for (int i = 0; i < 1023; ++i) {
        Item item;
        EXPECT_TRUE(q.pop(item));
        EXPECT_EQ(q.size(), 1023 - i - 1);
    }
}

TEST(SPSCQueueTest, EmptyTest) {
    SPSCQueue<Item, 1024> q;
    EXPECT_EQ(q.empty(), true);
    for (int i = 0; i < 1023; ++i) {
        q.emplace(i, i + 1);
        EXPECT_EQ(q.empty(), false);
    }

    for (int i = 0; i < 1023; ++i) {
        EXPECT_EQ(q.empty(), false);
        Item item;
        q.pop(item);
    }
    EXPECT_EQ(q.empty(), true);
}