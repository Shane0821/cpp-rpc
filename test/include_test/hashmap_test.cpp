#include "hashmap.h"

#include <gtest/gtest.h>

#include <iostream>

TEST(HashMapTest, Insert) {
    HashMap<int, int> map;
    map.insert(1, 2);
    map.insert(2, 3);
    map.insert(3, 4);
    map.insert(4, 5);
    map.insert(5, 6);
    map.insert(6, 7);
    map.insert(7, 8);
    map.insert(8, 9);
    map.insert(9, 10);
    map.insert(10, 11);
    map.insert(11, 12);
    map[12] = 13;
    for (int i = 1; i <= 12; ++i) {
        auto value = map[i];
        ASSERT_EQ(value, i + 1);
    }
    ASSERT_EQ(map.size(), 12);
    ASSERT_EQ(map.capacity(), 16);

    map.insert(13, 14);
    for (int i = 1; i <= 13; ++i) {
        auto value = map[i];
        ASSERT_EQ(value, i + 1);
    }
    ASSERT_EQ(map.size(), 13);
    ASSERT_EQ(map.capacity(), 32);

    map.insert(45, 46);
    ASSERT_EQ(map[45], 46);
    ASSERT_EQ(map[13], 14);
    for (int i = 1; i <= 12; ++i) {
        auto value = map[i];
        ASSERT_EQ(value, i + 1);
    }
    ASSERT_EQ(map.size(), 14);

    map.insert(14, 15);
    ASSERT_EQ(map[14], 15);
    ASSERT_EQ(map[45], 46);
    ASSERT_EQ(map[13], 14);
    for (int i = 1; i <= 12; ++i) {
        auto value = map[i];
        ASSERT_EQ(value, i + 1);
    }
    ASSERT_EQ(map.size(), 15);
    ASSERT_EQ(map.capacity(), 32);
}

TEST(HashMapTest, Erase) {
    HashMap<int, int> map;
    map.insert(1, 2);
    map.insert(2, 3);
    map.insert(3, 4);
    map.insert(4, 5);
    map.insert(5, 6);
    map.insert(6, 7);
    map.insert(7, 8);
    map.insert(8, 9);
    map.insert(9, 10);
    map.insert(10, 11);
    map.insert(11, 12);
    map.insert(12, 13);
    for (int i = 1; i <= 12; ++i) {
        ASSERT_EQ(map[i], i + 1);
        ASSERT_TRUE(map.contains(i));
    }
    ASSERT_EQ(map.size(), 12);
    ASSERT_EQ(map.capacity(), 16);

    map.erase(1);
    map.erase(2);
    map.erase(3);
    map.erase(4);
    map.erase(5);
    map.erase(6);
    map.erase(7);
    map.erase(8);
    map.erase(9);
    map.erase(10);
    map.erase(11);
    map.erase(12);
    ASSERT_EQ(map.size(), 0);
    ASSERT_EQ(map.capacity(), 16);
    for (int i = 1; i <= 12; ++i) {
        ASSERT_FALSE(map.contains(i));
    }
}

TEST(HashMapTest, Copy) {
    HashMap<int, int> map;
    map.insert(1, 2);
    map.insert(2, 3);
    map.insert(3, 4);
    map.insert(4, 5);
    map.insert(5, 6);
    map.insert(6, 7);
    map.insert(7, 8);
    map.insert(8, 9);
    map.insert(9, 10);
    map.insert(10, 11);
    map.insert(11, 12);
    map.insert(12, 13);
    for (int i = 1; i <= 12; ++i) {
        auto value = map[i];
        ASSERT_EQ(value, i + 1);
    }
    ASSERT_EQ(map.size(), 12);
    ASSERT_EQ(map.capacity(), 16);

    auto map2 = map;
    for (int i = 1; i <= 12; ++i) {
        auto value = map2[i];
        ASSERT_EQ(value, i + 1);
    }
    ASSERT_EQ(map2.size(), 12);
    ASSERT_EQ(map2.capacity(), 16);
}

TEST(HashMapTest, Move) {
    HashMap<int, int> map;
    map.insert(1, 2);
    map.insert(2, 3);
    map.insert(3, 4);
    map.insert(4, 5);
    map.insert(5, 6);
    map.insert(6, 7);
    map.insert(7, 8);
    map.insert(8, 9);
    map.insert(9, 10);
    map.insert(10, 11);
    map.insert(11, 12);
    map.insert(12, 13);
    for (int i = 1; i <= 12; ++i) {
        auto value = map[i];
        ASSERT_EQ(value, i + 1);
    }
    ASSERT_EQ(map.size(), 12);
    ASSERT_EQ(map.capacity(), 16);

    auto map2 = std::move(map);

    for (int i = 1; i <= 12; ++i) {
        auto value = map2[i];
        ASSERT_EQ(value, i + 1);
    }
    ASSERT_EQ(map2.size(), 12);
    ASSERT_EQ(map2.capacity(), 16);

    ASSERT_EQ(map.size(), 0);
    ASSERT_EQ(map.capacity(), 0);

    for (int i = 1; i <= 12; ++i) {
        auto value = map[i];
        ASSERT_EQ(value, 0);
    }

    ASSERT_EQ(map.size(), 12);
    ASSERT_EQ(map.capacity(), 16);
}
