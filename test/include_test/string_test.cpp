#include <gtest/gtest.h>

#include "str.h"

TEST(StringTest, Constructor) {
    String s;
    EXPECT_EQ(s.size(), 0);
    EXPECT_EQ(s.capacity(), 15);
    EXPECT_TRUE(s.empty());

    String s2("Hello, world!");
    EXPECT_EQ(s2.size(), 13);
    EXPECT_EQ(s2.capacity(), 15);
    EXPECT_FALSE(s2.empty());
    EXPECT_STREQ(s2.c_str(), "Hello, world!");

    String s3("Hello, world!!!!");
    EXPECT_EQ(s3.size(), 16);
    EXPECT_EQ(s3.capacity(), 16);
    EXPECT_FALSE(s3.empty());
    EXPECT_STREQ(s3.c_str(), "Hello, world!!!!");
}

TEST(StringTest, CopyConstructor) {
    String s("Hello, world!");
    String s_copy = s;
    s.clear();
    EXPECT_EQ(s_copy.size(), 13);
    EXPECT_EQ(s_copy.capacity(), 15);
    EXPECT_FALSE(s_copy.empty());
    EXPECT_STREQ(s_copy.c_str(), "Hello, world!");

    String s2("Hello, world!!!!");
    String s2_copy = s2;
    s2.clear();
    EXPECT_EQ(s2_copy.size(), 16);
    EXPECT_EQ(s2_copy.capacity(), 16);
    EXPECT_FALSE(s2_copy.empty());
    EXPECT_STREQ(s2_copy.c_str(), "Hello, world!!!!");
}

TEST(StringTest, MoveConstructor) {
    String s("Hello, world!");
    String s_moved = std::move(s);
    EXPECT_EQ(s.size(), 0);
    EXPECT_EQ(s.capacity(), 15);
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(s_moved.size(), 13);
    EXPECT_EQ(s_moved.capacity(), 15);
    EXPECT_FALSE(s_moved.empty());
    EXPECT_STREQ(s_moved.c_str(), "Hello, world!");

    String s2("Hello, world!!!!");
    String s2_moved = std::move(s2);
    EXPECT_EQ(s2.size(), 0);
    EXPECT_EQ(s2.capacity(), 0);
    EXPECT_TRUE(s2.empty());
    EXPECT_EQ(s2_moved.size(), 16);
    EXPECT_EQ(s2_moved.capacity(), 16);
    EXPECT_FALSE(s2_moved.empty());
    EXPECT_STREQ(s2_moved.c_str(), "Hello, world!!!!");
}

TEST(StringTest, CopyAssignment) {
    String s("Hello, world!");
    String s_copy;
    s_copy = s;
    s.clear();
    EXPECT_EQ(s_copy.size(), 13);
    EXPECT_EQ(s_copy.capacity(), 15);
    EXPECT_FALSE(s_copy.empty());
    EXPECT_STREQ(s_copy.c_str(), "Hello, world!");

    String s2("Hello, world!!!!");
    String s2_copy;
    s2_copy = s2;
    s2.clear();
    EXPECT_EQ(s2_copy.size(), 16);
    EXPECT_EQ(s2_copy.capacity(), 16);
    EXPECT_FALSE(s2_copy.empty());
    EXPECT_STREQ(s2_copy.c_str(), "Hello, world!!!!");
}

TEST(StringTest, MoveAssignment) {
    String s("Hello, world!");
    String s_moved;
    s_moved = std::move(s);
    EXPECT_EQ(s.size(), 0);
    EXPECT_EQ(s.capacity(), 15);
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(s_moved.size(), 13);
    EXPECT_EQ(s_moved.capacity(), 15);
    EXPECT_FALSE(s_moved.empty());
    EXPECT_STREQ(s_moved.c_str(), "Hello, world!");

    String s2("Hello, world!!!!");
    String s2_moved;
    s2_moved = std::move(s2);
    EXPECT_EQ(s2.size(), 0);
    EXPECT_EQ(s2.capacity(), 0);
    EXPECT_TRUE(s2.empty());
    EXPECT_EQ(s2_moved.size(), 16);
    EXPECT_EQ(s2_moved.capacity(), 16);
    EXPECT_FALSE(s2_moved.empty());
    EXPECT_STREQ(s2_moved.c_str(), "Hello, world!!!!");
}

TEST(StringTest, Clear) {
    String s("Hello, world!");
    s.clear();
    EXPECT_EQ(s.size(), 0);
    EXPECT_EQ(s.capacity(), 15);
    EXPECT_TRUE(s.empty());

    s.append("Goodbye, world!");
    EXPECT_EQ(s.size(), 15);
    EXPECT_EQ(s.capacity(), 15);
    EXPECT_FALSE(s.empty());
    EXPECT_STREQ(s.c_str(), "Goodbye, world!");
}

TEST(StringTest, IndexOperator) {
    String s("Hello, world!");
    EXPECT_EQ(s[0], 'H');
    EXPECT_EQ(s[7], 'w');
    EXPECT_EQ(s[12], '!');
}

TEST(StringTest, At) {
    String s("Hello, world!");
    EXPECT_EQ(s.at(0), 'H');
    EXPECT_EQ(s.at(7), 'w');
    EXPECT_EQ(s.at(12), '!');
    EXPECT_THROW(s.at(13), std::out_of_range);
}

TEST(StringTest, Append) {
    String s("Hello, ");
    s.append("world!");
    EXPECT_EQ(s.size(), 13);
    EXPECT_EQ(s.capacity(), 15);
    EXPECT_FALSE(s.empty());
    EXPECT_STREQ(s.c_str(), "Hello, world!");

    s.append(" Goodbye, world!");
    EXPECT_EQ(s.size(), 29);
    EXPECT_EQ(s.capacity(), 29);
    EXPECT_FALSE(s.empty());
    EXPECT_STREQ(s.c_str(), "Hello, world! Goodbye, world!");
}

TEST(StringTest, PushBack) {
    String s("Hello, ");
    s.push_back('w');
    s.push_back('o');
    s.push_back('r');
    s.push_back('l');
    s.push_back('d');
    s.push_back('!');
    EXPECT_EQ(s.size(), 13);
    EXPECT_EQ(s.capacity(), 15);
    EXPECT_FALSE(s.empty());
    EXPECT_STREQ(s.c_str(), "Hello, world!");

    s.push_back('!');
    EXPECT_EQ(s.size(), 14);
    EXPECT_EQ(s.capacity(), 15);
    EXPECT_FALSE(s.empty());
    EXPECT_STREQ(s.c_str(), "Hello, world!!");

    s.push_back('!');
    EXPECT_EQ(s.size(), 15);
    EXPECT_EQ(s.capacity(), 15);
    EXPECT_FALSE(s.empty());
    EXPECT_STREQ(s.c_str(), "Hello, world!!!");

    s.push_back('!');
    EXPECT_EQ(s.size(), 16);
    EXPECT_EQ(s.capacity(), 30);
    EXPECT_FALSE(s.empty());
    EXPECT_STREQ(s.c_str(), "Hello, world!!!!");
}

TEST(StringTest, PopBack) {
    String s("Hello, world!");
    s.pop_back();
    EXPECT_EQ(s.size(), 12);
    EXPECT_EQ(s.capacity(), 15);
    EXPECT_FALSE(s.empty());
    EXPECT_STREQ(s.c_str(), "Hello, world");
}

TEST(StringTest, Reserve) {
    String s("Hello, world!");
    s.reserve(100);
    EXPECT_EQ(s.size(), 13);
    EXPECT_EQ(s.capacity(), 100);
    EXPECT_FALSE(s.empty());
    EXPECT_STREQ(s.c_str(), "Hello, world!");

    s.reserve(10);
    EXPECT_EQ(s.size(), 13);
    EXPECT_EQ(s.capacity(), 100);
    EXPECT_FALSE(s.empty());
    EXPECT_STREQ(s.c_str(), "Hello, world!");
}

TEST(StringTest, Concatenation) {
    String s1("Hello, ");
    String s2("world!");

    String s3 = s1 + s2;
    EXPECT_EQ(s3.size(), 13);
    EXPECT_EQ(s3.capacity(), 15);
    EXPECT_FALSE(s3.empty());
    EXPECT_STREQ(s3.c_str(), "Hello, world!");

    String s4 = s3 + "!!!";
    EXPECT_EQ(s4.size(), 16);
    EXPECT_EQ(s4.capacity(), 16);
    EXPECT_FALSE(s4.empty());
    EXPECT_STREQ(s4.c_str(), "Hello, world!!!!");
}

TEST(StringTest, ConcatenationAssignment) {
    String s1("Hello, ");
    String s2("world!");
    s1 += s2;
    EXPECT_EQ(s1.size(), 13);
    EXPECT_EQ(s1.capacity(), 15);
    EXPECT_FALSE(s1.empty());
    EXPECT_STREQ(s1.c_str(), "Hello, world!");

    s1 += '!';
    EXPECT_EQ(s1.size(), 14);
    EXPECT_EQ(s1.capacity(), 15);
    EXPECT_FALSE(s1.empty());
    EXPECT_STREQ(s1.c_str(), "Hello, world!!");

    s1 += " Goodbye, world!";
    EXPECT_EQ(s1.size(), 30);
    EXPECT_EQ(s1.capacity(), 30);
    EXPECT_FALSE(s1.empty());
    EXPECT_STREQ(s1.c_str(), "Hello, world!! Goodbye, world!");
}

TEST(StringTest, Comparison) {
    String s1("Hello, world!");
    String s2("Hello, world!");
    String s3("Goodbye, world!");
    String s4("Hello, world!!");
    String s5("Hello, worl");
    String s6("Hello, world! ");
    String s7("Hello, world");

    EXPECT_TRUE(s1 == s2);
    EXPECT_FALSE(s1 == s3);
    EXPECT_FALSE(s1 == s4);
    EXPECT_FALSE(s1 == s5);
    EXPECT_FALSE(s1 == s6);
    EXPECT_FALSE(s1 == s7);

    EXPECT_FALSE(s1 != s2);
    EXPECT_TRUE(s1 != s3);
    EXPECT_TRUE(s1 != s4);
    EXPECT_TRUE(s1 != s5);
    EXPECT_TRUE(s1 != s6);
    EXPECT_TRUE(s1 != s7);

    EXPECT_FALSE(s1 < s2);
    EXPECT_TRUE(s1 > s3);
    EXPECT_TRUE(s1 < s4);
    EXPECT_TRUE(s1 > s5);
    EXPECT_TRUE(s1 < s6);
    EXPECT_TRUE(s1 > s7);
}
