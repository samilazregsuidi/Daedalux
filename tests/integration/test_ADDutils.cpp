#include <gtest/gtest.h>
#include <daedalux/feature/ADDutils.hpp>  // include the header, not the .cpp

TEST(ADDUtilsTests, impliesTest) {
    // Test case 1: a implies b
    Cudd mgr(0,0);
    BDD x = mgr.bddVar();
    BDD y = mgr.bddVar();
    BDD f = x * y;
    BDD g = y + !x;
         
    bool result1 = f <= g;
    EXPECT_TRUE(result1);

    bool result2 = implies(f.Add(), g.Add());
    EXPECT_TRUE(result2);
}
