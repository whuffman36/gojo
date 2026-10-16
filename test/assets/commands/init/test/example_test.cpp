#include <gtest/gtest.h>


// Demonstrate some basic assertions.
// Taken directly from GoogleTest website.
TEST(TestSuiteName, TestName) {
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
}
