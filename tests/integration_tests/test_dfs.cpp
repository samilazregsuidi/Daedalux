#include "../../src/algorithm/explore.hpp"
#include "../../src/core/automata/fsmEdge.hpp"
#include "../../src/core/automata/fsmNode.hpp"
#include <gtest/gtest.h>

class ExploreTest : public ::testing::Test {
protected:
  void SetUp() override
  {
    // Common setup code that will be called before each test
    symTable * table = new symTable("global", nullptr);
    myFSM = std::make_unique<fsm>(table, ADD());
    fsmNode * node1 = myFSM->createFsmNode(0, 1);
    fsmNode * node2 = myFSM->createFsmNode(1, 2);
    fsmNode * node3 = myFSM->createFsmNode(2, 3);

    // Create some fsmEdge objects
    fsmEdge * edge1 = new fsmEdge(node1, node2, nullptr, 10, false);
    fsmEdge * edge2 = new fsmEdge(node2, node3, nullptr, 20, false);
    fsmEdge * edge3 = new fsmEdge(node1, node3, nullptr, 30, false);

    node1->addTransition(edge1);
    node2->addTransition(edge2);
    node1->addTransition(edge3);

    node2->addInputTransition(edge1);
    node3->addInputTransition(edge2);
    node3->addInputTransition(edge3);

    // Add the edges to the FSM
    myFSM->addTransition(edge1);
    myFSM->addTransition(edge2);
    myFSM->addTransition(edge3);
  }
  void TearDown() override
  {
    // Common teardown code that will be called after each test
  }

  std::unique_ptr<fsm> myFSM;
};

// Test case for the explore function
TEST_F(ExploreTest, Explore)
{
  launchExecution(myFSM.get());
  // Check that the FSM has been explored
  EXPECT_EQ(myFSM->getNodes().size(), 3);
  EXPECT_EQ(myFSM->getTransitions().size(), 3);
  EXPECT_EQ(myFSM->getNode(1)->getEdges().size(), 2);
  EXPECT_EQ(myFSM->getNode(2)->getEdges().size(), 1);
  EXPECT_EQ(myFSM->getNode(3)->getEdges().size(), 0);
}