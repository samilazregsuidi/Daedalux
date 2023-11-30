#include <gtest/gtest.h>
#include "../../src/visualizers/trace.hpp"
#include "../../src/semantic/compositeState.hpp"
#include "../../src/semantic/compositeTransition.hpp"

class TraceTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Common setup code that will be called before each test
        myTrace = new trace();
    }
    void TearDown() override {
        // Common teardown code that will be called after each test
        delete myTrace;
    }
    trace* myTrace;
};

// Test case for adding transitions
TEST_F(TraceTest, AddTransition) {
  std::list<transition*> transList = std::list<transition*>();
  auto state = new compState("test_variable");
  transition* t = new compTransition(state, transList);
  myTrace->addTransition(t);
  ASSERT_EQ(myTrace->size(), 1);
}

// Test case for adding states
TEST_F(TraceTest, AddState) {
  auto state = new compState("test_variable");
  myTrace->addState(state);
  ASSERT_EQ(myTrace->getStates().size(), 1);
}

// Test case for adding trace
TEST_F(TraceTest, AddTrace) {
  trace* other = new trace();
  std::list<transition*> transList = std::list<transition*>();
  auto state = new compState("test_variable");
  transition* t = new compTransition(state, transList);
  other->addTransition(t);
  other->addState(state);
  myTrace->addTrace(other);
  ASSERT_EQ(myTrace->size(), 1);
  ASSERT_EQ(myTrace->getStates().size(), 1);
}

// Test case for equality operator
TEST_F(TraceTest, EqualityOperator) {
  trace t1 = trace();
  trace t2 = trace();
  ASSERT_TRUE(t1 == t2);
}

TEST_F(TraceTest, InequalityOperatorAdvanced) {
  trace t1 = trace();
  trace t2 = trace();

  std::list<transition*> transList = std::list<transition*>();
  auto state = new compState("test_variable");
  transition* t = new compTransition(state, transList);
  t1.addTransition(t);
  t2.addState(state);
  ASSERT_TRUE(t1 != t2);
}


// Test case for inequality operator
TEST_F(TraceTest, InequalityOperator) {
  trace t1 = trace();
  trace t2 = trace();
  ASSERT_FALSE(t1 != t2);
}

// Test case for printing CSV 
// TODO: Fix this test case
TEST_F(TraceTest, PrintCSV) {
  std::list<transition*> transList = std::list<transition*>();
  auto state = new compState("test_variable");
  myTrace->addState(state);
  std::stringstream ss;
  myTrace->printCSV(ss);
  ASSERT_EQ(ss.str(), " ..\n"); 
}