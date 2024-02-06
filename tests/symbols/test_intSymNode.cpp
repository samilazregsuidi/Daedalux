#include <gtest/gtest.h>
#include "../../src/promela/symbol/vardef/intSymNode.hpp"

class IntSymNodeTestFixture : public ::testing::Test {
protected:
    void SetUp() override {
        // Common setup code that will be called before each test
        intNode = new intSymNode(0, "myVar", 0, nullptr);
    }

    void TearDown() override {
        // Common teardown code that will be called after each test
        delete intNode;
    }

    intSymNode* intNode;
};

TEST_F(IntSymNodeTestFixture, GetBasicTypeNameTest) {
    // Access intNode using the member variable
    EXPECT_EQ(intNode->getBasicTypeName(), "int");
}

TEST_F(IntSymNodeTestFixture, GetTypeSizeTest) {
    // Access intNode using the member variable
    EXPECT_EQ(intNode->getTypeSize(), 4);
}

TEST_F(IntSymNodeTestFixture, GetUpperBoundTest) {
    // Access intNode using the member variable
    EXPECT_EQ(intNode->getUpperBound(), std::numeric_limits<int>::max());
}

TEST_F(IntSymNodeTestFixture, GetLowerBoundTest) {
    // Access intNode using the member variable
    EXPECT_EQ(intNode->getLowerBound(), std::numeric_limits<int>::min());
}