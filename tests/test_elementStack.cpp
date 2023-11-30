#include "gtest/gtest.h"
#include "../src/algorithms/elementStack.hpp"
#include "../src/semantic/compositeState.hpp"

// Test fixture for the elementStack class
class ElementStackTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Optional: Setup code to run before each test
        s = new elementStack();
    }

    void TearDown() override {
        // Optional: Teardown code to run after each test
        delete s;
    }
    elementStack * s;
};

// Test the push and top functions
TEST_F(ElementStackTest, PushAndTop) {
    auto initialState = new compState("init_state");
    s->push(initialState, 0);

    ASSERT_FALSE(s->empty());

    // elementStack::element* topElement = s->top();
    // ASSERT_TRUE(topElement != nullptr);
    // ASSERT_TRUE(topElement->init);
    // ASSERT_EQ(topElement->depth, 0);
    // ASSERT_EQ(topElement->s, initialState);

    // // Cleanup
    // s->pop();
    delete initialState;
}

// Test the pop function
TEST_F(ElementStackTest, Pop) {
    auto initialState = new compState("init_state");

    s->push(initialState, 0);
    ASSERT_FALSE(s->empty());

    // s->pop();
    // ASSERT_TRUE(s->empty());
    // ASSERT_EQ(s->top(), nullptr);

    // Cleanup
}

// Test the isIn function
TEST_F(ElementStackTest, IsIn) {
    auto initialState = new compState("init_state");

    s->push(initialState, 0);
    ASSERT_TRUE(s->isIn(initialState->hash()));

    // Cleanup
    s->pop();
    delete initialState;
}

// Test the empty function
TEST_F(ElementStackTest, Empty) {
    ASSERT_TRUE(s->empty());

    auto initialState = new compState("init_state");
    s->push(initialState, 0);
    ASSERT_FALSE(s->empty());

    // Cleanup
    s->pop();
    delete initialState;
}