#include <gtest/gtest.h>
#include "../src/semantic/compositeState.hpp"

class StateTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Common setup code for all tests goes here
    }

    void TearDown() override {
        // Common teardown code for all tests goes here
    }
};

TEST_F(StateTest, DefaultConstructor) {
    auto s = new compState("testState");
    
    EXPECT_EQ(variable::Type::V_COMP_S, s->getType());
    EXPECT_EQ("testState", s->getFullName());
    EXPECT_EQ(1.0, s->getProbability());
    EXPECT_EQ(nullptr, s->getOrigin());
    EXPECT_EQ(0u, s->getErrorMask());
    EXPECT_TRUE(s->executables().empty());
    EXPECT_TRUE(s->hasDeadlock());
    EXPECT_EQ(0u, s->getErrorMask());

    delete s;
}

TEST_F(StateTest, CopyConstructor) {
    // Create a state object to copy from
    auto originalState = new compState("test_variable");
    originalState->prob = 0.5;

    // Create a new state object using the copy constructor
    // auto copiedState = new compState(originalState);

    // Verify that the copied state has the same values as the original state
    // EXPECT_EQ(copiedState->prob, originalState->prob);
    // EXPECT_EQ(copiedState->origin, originalState->origin);
    // EXPECT_EQ(copiedState->errorMask, originalState->errorMask);

    // delete copiedState;
    // delete originalState;
}

// TEST_F(StateTest, CopyConstructorWithErrors) {
//     auto original = new compState("originalState");
//     original->addError(state::ERR_DEADLOCK);

//     auto copy = new compState(original);

//     EXPECT_EQ(original->getType(), copy->getType());
//     EXPECT_EQ(original->getFullName(), copy->getFullName());
//     EXPECT_EQ(original->getProbability(), copy->getProbability());
//     EXPECT_EQ(nullptr, copy->getOrigin()); // Origin should be reset in the copy
//     EXPECT_EQ(original->getErrorMask(), copy->getErrorMask());
//     EXPECT_FALSE(copy->hasDeadlock()); // A copy should not inherit executables
//     EXPECT_EQ(state::ERR_DEADLOCK, copy->getErrorMask());

//     delete copy;
//     delete original;
// }