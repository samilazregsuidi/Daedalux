#include <gtest/gtest.h>
#include "../src/features/ADDutils.cpp"

TEST(ADDUtilsTests, impliesTest) {
    // Test case 1: a implies b
    ADD a = ADD();
    ADD b = ADD();
    bool result1 = implies(a, b);
    EXPECT_TRUE(result1);

    // Test case 2: a does not imply b
    ADD c = ADD(); // initialize ADD c
    ADD d = ADD(); // initialize ADD d
    bool result2 = implies(c, d);
    EXPECT_FALSE(result2);
}
