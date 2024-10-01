#include <gtest/gtest.h>

#include "spsc_queue.h"

TEST(SPSCQueueTest, PushPop) {
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
}