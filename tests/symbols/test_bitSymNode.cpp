#include <gtest/gtest.h>
#include "../../src/promela/symbol/vardef/bitSymNode.hpp"

class BitSymNodeTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Common setup code that will be called before each test
        bitNode = new bitSymNode(1, "bitVar", 0);
    }

    void TearDown() override {
        // Optional: Teardown code to run after each test case
        delete bitNode;
    }

    bitSymNode* bitNode;

};

// Test the constructor of bitSymNode
TEST_F(BitSymNodeTest, ConstructorTest) {
    EXPECT_EQ(bitNode->getType(), symbol::T_BIT);
    EXPECT_EQ(bitNode->getLineNb(), 1);
    EXPECT_EQ(bitNode->getName(), "bitVar");
    EXPECT_EQ(bitNode->getUpperBound(), 1);
    EXPECT_EQ(bitNode->getLowerBound(), 0);
    EXPECT_EQ(bitNode->getInitExpr(), nullptr);
}