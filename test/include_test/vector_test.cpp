#include "vector.h"

#include <gtest/gtest.h>

TEST(VectorTest, Constructor) {
    Vector<int> v;
    EXPECT_EQ(v.size(), 0);
    EXPECT_EQ(v.capacity(), 0);
    EXPECT_TRUE(v.empty());

    Vector<int> v2(10);
    EXPECT_EQ(v2.size(), 10);
    EXPECT_EQ(v2.capacity(), 10);
    EXPECT_FALSE(v2.empty());

    Vector<int> v3(10, 42);
    EXPECT_EQ(v3.size(), 10);
    EXPECT_EQ(v3.capacity(), 10);
    EXPECT_FALSE(v3.empty());
    for (size_t i = 0; i < v3.size(); ++i) {
        EXPECT_EQ(v3[i], 42);
    }
}

TEST(VectorTest, CopyConstructor) {
    Vector<int> v;
    v.push_back(10);
    v.push_back(20);
    v.push_back(30);

    Vector<int> v_copy = v;
    EXPECT_EQ(v_copy.size(), 3);
    EXPECT_EQ(v_copy.capacity(), 4);
    EXPECT_FALSE(v_copy.empty());
    EXPECT_EQ(v_copy[0], 10);
    EXPECT_EQ(v_copy[1], 20);
    EXPECT_EQ(v_copy[2], 30);
}

TEST(VectorTest, MoveConstructor) {
    Vector<int> v;
    v.push_back(10);
    v.push_back(20);
    v.push_back(30);

    Vector<int> v_moved = std::move(v);
    EXPECT_EQ(v.size(), 0);
    EXPECT_EQ(v.capacity(), 0);
    EXPECT_TRUE(v.empty());
    EXPECT_EQ(v_moved.size(), 3);
    EXPECT_EQ(v_moved.capacity(), 4);
    EXPECT_FALSE(v_moved.empty());
    EXPECT_EQ(v_moved[0], 10);
    EXPECT_EQ(v_moved[1], 20);
    EXPECT_EQ(v_moved[2], 30);
}

TEST(VectorTest, PushBack) {
    Vector<int> v;
    v.push_back(10);
    v.push_back(20);
    v.push_back(30);
    EXPECT_EQ(v.size(), 3);
    EXPECT_EQ(v.capacity(), 4);
    EXPECT_FALSE(v.empty());
    EXPECT_EQ(v[0], 10);
    EXPECT_EQ(v[1], 20);
    EXPECT_EQ(v[2], 30);
}

TEST(VectorTest, EmplaceBack) {
    Vector<int> v;
    v.emplace_back(10);
    v.emplace_back(20);
    v.emplace_back(30);
    EXPECT_EQ(v.size(), 3);
    EXPECT_EQ(v.capacity(), 4);
    EXPECT_FALSE(v.empty());
    EXPECT_EQ(v[0], 10);
    EXPECT_EQ(v[1], 20);
    EXPECT_EQ(v[2], 30);
}

TEST(VectorTest, PopBack) {
    Vector<int> v;
    v.push_back(10);
    v.push_back(20);
    v.push_back(30);
    v.pop_back();
    EXPECT_EQ(v.size(), 2);
    EXPECT_EQ(v.capacity(), 4);
    EXPECT_FALSE(v.empty());
    EXPECT_EQ(v[0], 10);
    EXPECT_EQ(v[1], 20);
}

TEST(VectorTest, Clear) {
    Vector<int> v;
    v.push_back(10);
    v.push_back(20);
    v.push_back(30);
    v.clear();
    EXPECT_EQ(v.size(), 0);
    EXPECT_EQ(v.capacity(), 4);
    EXPECT_TRUE(v.empty());
}

TEST(VectorTest, IndexOperator) {
    Vector<int> v;
    v.push_back(10);
    v.push_back(20);
    v.push_back(30);
    EXPECT_EQ(v[0], 10);
    EXPECT_EQ(v[1], 20);
    EXPECT_EQ(v[2], 30);
}

TEST(VectorTest, At) {
    Vector<int> v;
    v.push_back(10);
    v.push_back(20);
    v.push_back(30);
    EXPECT_EQ(v.at(0), 10);
    EXPECT_EQ(v.at(1), 20);
    EXPECT_EQ(v.at(2), 30);
    EXPECT_THROW(v.at(3), std::out_of_range);
}

TEST(VectorTest, Back) {
    Vector<int> v;
    v.push_back(10);
    v.push_back(20);
    v.push_back(30);
    EXPECT_EQ(v.back(), 30);
    v.pop_back();
    EXPECT_EQ(v.back(), 20);
    v.pop_back();
    EXPECT_EQ(v.back(), 10);
    v.pop_back();
    EXPECT_THROW(v.back(), std::out_of_range);
}

TEST(VectorTest, Reserve) {
    Vector<int> v;
    v.reserve(10);
    EXPECT_EQ(v.size(), 0);
    EXPECT_EQ(v.capacity(), 10);
    v.push_back(10);
    v.push_back(20);
    v.push_back(30);
    v.reserve(20);
    EXPECT_EQ(v.size(), 3);
    EXPECT_EQ(v.capacity(), 20);
    EXPECT_EQ(v[0], 10);
    EXPECT_EQ(v[1], 20);
    EXPECT_EQ(v[2], 30);
}

TEST(VectorTest, Resize) {
    Vector<int> v;
    v.resize(10);
    EXPECT_EQ(v.size(), 10);
    EXPECT_EQ(v.capacity(), 10);
    for (size_t i = 0; i < v.size(); ++i) {
        EXPECT_EQ(v[i], 0);
    }
    v.resize(5);
    EXPECT_EQ(v.size(), 5);
    EXPECT_EQ(v.capacity(), 10);
    v.resize(15, 42);
    EXPECT_EQ(v.size(), 15);
    EXPECT_EQ(v.capacity(), 15);
    for (size_t i = 0; i < 5; ++i) {
        EXPECT_EQ(v[i], 0);
    }
    for (size_t i = 5; i < 15; ++i) {
        EXPECT_EQ(v[i], 42);
    }
}