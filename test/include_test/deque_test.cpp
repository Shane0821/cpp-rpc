#include "deque.h"

#include <gtest/gtest.h>

TEST(DequeTest, PushBack) {
    Deque<int, 4> deque;
    for (int i = 0; i < 9; i++) {
        deque.push_back(i + 1);
    }
    ASSERT_EQ(deque.size(), 9);
    ASSERT_TRUE(!deque.empty());

    for (int i = 0; i < 9; i++) {
        ASSERT_EQ(deque[i], i + 1);
    }

    ASSERT_THROW(deque[9], std::out_of_range);
    ASSERT_EQ(deque.front(), 1);
    ASSERT_EQ(deque.back(), 9);
}

TEST(DequeTest, PushBackDense) {
    Deque<int, 8> deque;
    for (int i = 0; i < 10000; i++) {
        deque.push_back(i + 1);
    }
    ASSERT_EQ(deque.size(), 10000);
    ASSERT_TRUE(!deque.empty());

    for (int i = 0; i < 10000; i++) {
        ASSERT_EQ(deque[i], i + 1);
    }

    ASSERT_THROW(deque[10000], std::out_of_range);
    ASSERT_EQ(deque.front(), 1);
    ASSERT_EQ(deque.back(), 10000);
}

TEST(DequeTest, EmplaceBack) {
    Deque<int, 4> deque;
    for (int i = 0; i < 9; i++) {
        deque.emplace_back(i + 1);
    }
    ASSERT_EQ(deque.size(), 9);
    ASSERT_TRUE(!deque.empty());
    for (int i = 0; i < 9; i++) {
        ASSERT_EQ(deque[i], i + 1);
    }
    ASSERT_EQ(deque.front(), 1);
    ASSERT_EQ(deque.back(), 9);
}

TEST(DequeTest, PushFront) {
    Deque<int, 4> deque;
    for (int i = 0; i < 9; i++) {
        deque.push_front(i + 1);
    }
    ASSERT_EQ(deque.size(), 9);
    ASSERT_TRUE(!deque.empty());
    for (int i = 0; i < 9; i++) {
        ASSERT_EQ(deque[i], 9 - i);
    }
    ASSERT_EQ(deque.front(), 9);
    ASSERT_EQ(deque.back(), 1);
}

TEST(DequeTest, PushFrontDense) {
    Deque<int, 8> deque;
    for (int i = 0; i < 10000; i++) {
        deque.push_front(i + 1);
    }
    ASSERT_EQ(deque.size(), 10000);
    ASSERT_TRUE(!deque.empty());

    for (int i = 0; i < 10000; i++) {
        ASSERT_EQ(deque[i], 10000 - i);
    }

    ASSERT_THROW(deque[10000], std::out_of_range);
    ASSERT_EQ(deque.front(), 10000);
    ASSERT_EQ(deque.back(), 1);
}

TEST(DequeTest, EmplaceFront) {
    Deque<int, 4> deque;
    for (int i = 0; i < 9; i++) {
        deque.emplace_front(i + 1);
    }
    ASSERT_EQ(deque.size(), 9);
    ASSERT_TRUE(!deque.empty());
    for (int i = 0; i < 9; i++) {
        ASSERT_EQ(deque[i], 9 - i);
    }
    ASSERT_EQ(deque.front(), 9);
    ASSERT_EQ(deque.back(), 1);
}

TEST(DequeTest, Push) {
    Deque<int, 4> deque;
    for (int i = 0; i < 9; i++) {
        deque.push_back(i + 1);
    }
    for (int i = 0; i < 9; i++) {
        deque.push_front(i + 1);
    }
    ASSERT_EQ(deque.size(), 18);
    ASSERT_TRUE(!deque.empty());
    for (int i = 0; i < 9; i++) {
        ASSERT_EQ(deque[i], 9 - i);
    }
    for (int i = 0; i < 9; i++) {
        ASSERT_EQ(deque[i + 9], i + 1);
    }
    ASSERT_EQ(deque.front(), 9);
    ASSERT_EQ(deque.back(), 9);
}

TEST(DequeTest, PopBack) {
    Deque<int, 4> deque;
    for (int i = 0; i < 9; i++) {
        deque.push_back(i + 1);
    }
    for (int i = 0; i < 9; i++) {
        ASSERT_EQ(deque.back(), 9 - i);
        deque.pop_back();
    }
    ASSERT_EQ(deque.size(), 0);
    ASSERT_TRUE(deque.empty());
}

TEST(DequeTest, PopFront) {
    Deque<int, 4> deque;
    for (int i = 0; i < 9; i++) {
        deque.push_back(i + 1);
    }
    for (int i = 0; i < 9; i++) {
        ASSERT_EQ(deque.front(), i + 1);
        deque.pop_front();
    }
    ASSERT_EQ(deque.size(), 0);
    ASSERT_TRUE(deque.empty());
}

TEST(DequeTest, PushPopDense) {
    Deque<int, 8> deque;
    for (int i = 0; i < 10000; i++) {
        deque.push_back(i + 1);
    }
    for (int i = 0; i < 10000; i++) {
        ASSERT_EQ(deque.back(), 10000 - i);
        deque.pop_back();
    }
    ASSERT_EQ(deque.size(), 0);
    ASSERT_TRUE(deque.empty());

    for (int i = 0; i < 10000; i++) {
        deque.push_front(i + 1);
    }
    for (int i = 0; i < 10000; i++) {
        ASSERT_EQ(deque.back(), i + 1);
        ASSERT_EQ(deque.front(), 10000);
        deque.pop_back();
    }
    ASSERT_EQ(deque.size(), 0);
    ASSERT_TRUE(deque.empty());
}

TEST(DequeTest, Iterator) {
    Deque<int, 4> deque;
    for (int i = 0; i < 10000; i++) {
        deque.push_back(i + 1);
    }
    int i = 1;
    for (auto it = deque.begin(); it != deque.end(); ++it) {
        ASSERT_EQ(*it, i++);
    }
    ASSERT_EQ(i, 10001);
}

TEST(DequeTest, Copy) {
    Deque<int, 4> deque;
    for (int i = 0; i < 3; i++) {
        deque.push_back(i + 1);
    }
    Deque<int, 4> copy = deque;
    for (int i = 0; i < 3; i++) {
        ASSERT_EQ(copy[i], i + 1);
    }
    ASSERT_EQ(copy.size(), 3);
    ASSERT_TRUE(!copy.empty());
    ASSERT_EQ(copy.front(), 1);
    ASSERT_EQ(copy.back(), 3);

    for (int i = 3; i < 9; i++) {
        deque.push_back(i + 1);
    }
    for (int i = 0; i < 9; i++) {
        ASSERT_EQ(deque[i], i + 1);
    }
    for (int i = 0; i < 3; i++) {
        ASSERT_EQ(copy[i], i + 1);
    }
    ASSERT_EQ(copy.size(), 3);
    ASSERT_TRUE(!copy.empty());
    ASSERT_EQ(copy.front(), 1);
    ASSERT_EQ(copy.back(), 3);

    Deque<int, 4> copy2 = deque;
    for (int i = 0; i < 9; i++) {
        ASSERT_EQ(copy2[i], i + 1);
    }
    ASSERT_EQ(copy2.size(), 9);
    ASSERT_TRUE(!copy2.empty());
    ASSERT_EQ(copy2.front(), 1);
    ASSERT_EQ(copy2.back(), 9);
}

TEST(DequeTest, CopyAssign) {
    Deque<int, 4> deque;
    for (int i = 0; i < 3; i++) {
        deque.push_back(i + 1);
    }
    Deque<int, 4> copy;
    copy = deque;
    for (int i = 0; i < 3; i++) {
        ASSERT_EQ(copy[i], i + 1);
    }
    ASSERT_EQ(copy.size(), 3);
    ASSERT_TRUE(!copy.empty());
    ASSERT_EQ(copy.front(), 1);
    ASSERT_EQ(copy.back(), 3);

    for (int i = 3; i < 9; i++) {
        deque.push_back(i + 1);
    }
    for (int i = 0; i < 9; i++) {
        ASSERT_EQ(deque[i], i + 1);
    }
    for (int i = 0; i < 3; i++) {
        ASSERT_EQ(copy[i], i + 1);
    }
    ASSERT_EQ(copy.size(), 3);
    ASSERT_TRUE(!copy.empty());
    ASSERT_EQ(copy.front(), 1);
    ASSERT_EQ(copy.back(), 3);

    Deque<int, 4> copy2;
    copy2 = deque;
    for (int i = 0; i < 9; i++) {
        ASSERT_EQ(copy2[i], i + 1);
    }
    ASSERT_EQ(copy2.size(), 9);
    ASSERT_TRUE(!copy2.empty());
    ASSERT_EQ(copy2.front(), 1);
    ASSERT_EQ(copy2.back(), 9);
}

TEST(DequeTest, Move) {
    Deque<int, 4> deque;
    for (int i = 0; i < 9; i++) {
        deque.push_back(i + 1);
    }
    Deque<int, 4> copy = std::move(deque);
    for (int i = 0; i < 9; i++) {
        ASSERT_EQ(copy[i], i + 1);
    }
    ASSERT_EQ(copy.size(), 9);
    ASSERT_TRUE(!copy.empty());
    ASSERT_EQ(copy.front(), 1);
    ASSERT_EQ(copy.back(), 9);
}

TEST(DequeTest, MoveAssign) {
    Deque<int, 4> deque;
    for (int i = 0; i < 9; i++) {
        deque.push_back(i + 1);
    }
    Deque<int, 4> copy;
    copy = std::move(deque);
    for (int i = 0; i < 9; i++) {
        ASSERT_EQ(copy[i], i + 1);
    }
    ASSERT_EQ(copy.size(), 9);
    ASSERT_TRUE(!copy.empty());
    ASSERT_EQ(copy.front(), 1);
    ASSERT_EQ(copy.back(), 9);

    ASSERT_EQ(deque.size(), 0);
    ASSERT_TRUE(deque.empty());
}

TEST(DequeTest, Erase) {
    Deque<int, 4> deque;
    for (int i = 0; i < 9; i++) {
        deque.push_back(i + 1);
    }
    deque.erase(4);
    // 1 2 3 4 6 7 8 9
    ASSERT_EQ(deque.size(), 8);
    ASSERT_EQ(deque[4], 6);
    for (int i = 0; i < 4; i++) {
        ASSERT_EQ(deque[i], i + 1);
    }
    for (int i = 4; i < 8; i++) {
        ASSERT_EQ(deque[i], i + 2);
    }

    // 2 3 4 6 7 8 9
    deque.erase(0);
    ASSERT_EQ(deque.size(), 7);
    ASSERT_EQ(deque[0], 2);
    ASSERT_EQ(deque[1], 3);
    ASSERT_EQ(deque[2], 4);
    ASSERT_EQ(deque[3], 6);
    ASSERT_EQ(deque[4], 7);
    ASSERT_EQ(deque[5], 8);
    ASSERT_EQ(deque[6], 9);

    // 2 3 4 6 7 8
    deque.erase(6);
    ASSERT_EQ(deque.size(), 6);
    ASSERT_EQ(deque[0], 2);
    ASSERT_EQ(deque[1], 3);
    ASSERT_EQ(deque[2], 4);
    ASSERT_EQ(deque[3], 6);
    ASSERT_EQ(deque[4], 7);
    ASSERT_EQ(deque[5], 8);

    // 2 3 6 7 8
    deque.erase(2);
    ASSERT_EQ(deque.size(), 5);
    ASSERT_EQ(deque[0], 2);
    ASSERT_EQ(deque[1], 3);
    ASSERT_EQ(deque[2], 6);
    ASSERT_EQ(deque[3], 7);
    ASSERT_EQ(deque[4], 8);

    for (int i = 0; i < 5; i++) {
        deque.erase(0);
    }
    ASSERT_EQ(deque.size(), 0);
    ASSERT_TRUE(deque.empty());
}

TEST(DequeTest, EraseDense) {
    Deque<int, 8> deque;
    for (int i = 0; i < 10000; i++) {
        deque.push_back(i);
    }
    deque.erase(5111);
    ASSERT_EQ(deque.size(), 9999);
    ASSERT_EQ(deque[5111], 5112);
    for (int i = 0; i < 5111; i++) {
        ASSERT_EQ(deque[i], i);
    }
    for (int i = 5111; i < 9999; i++) {
        ASSERT_EQ(deque[i], i + 1);
    }

    deque.erase(1000);
    ASSERT_EQ(deque.size(), 9998);
    ASSERT_EQ(deque[1000], 1001);
    for (int i = 0; i < 1000; i++) {
        ASSERT_EQ(deque[i], i);
    }
    for (int i = 1000; i < 5110; i++) {
        ASSERT_EQ(deque[i], i + 1);
    }
    for (int i = 5110; i < 9998; i++) {
        ASSERT_EQ(deque[i], i + 2);
    }

    deque.erase(8997);
    ASSERT_EQ(deque.size(), 9997);
    for (int i = 0; i < 1000; i++) {
        ASSERT_EQ(deque[i], i);
    }
    for (int i = 1000; i < 5110; i++) {
        ASSERT_EQ(deque[i], i + 1);
    }
    for (int i = 5110; i < 8997; i++) {
        ASSERT_EQ(deque[i], i + 2);
    }
    for (int i = 8997; i < 9997; i++) {
        ASSERT_EQ(deque[i], i + 3);
    }

    for (int i = 0; i < 9997; i++) {
        deque.erase(9996 - i);
    }
    ASSERT_EQ(deque.size(), 0);
    ASSERT_TRUE(deque.empty());
}