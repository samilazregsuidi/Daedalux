#include <gtest/gtest.h>
#include "../src/automata/fsm.hpp"

#include "../src/automata/fsmEdge.cpp"

class FsmEdgeTest : public ::testing::Test {
protected:
    void SetUp() override {
        symTable* table = new symTable("global", nullptr);
        myFSM = new fsm(table, ADD());
    }

    void TearDown() override {
        delete myFSM;
    }

    fsm* myFSM;
};

TEST_F(FsmEdgeTest, ConstructorTest) {
    // Test case: Create an fsmEdge with valid arguments
    fsmNode * sourceNode = myFSM->createFsmNode(0, 1);
    fsmNode * targetNode = myFSM->createFsmNode(1, 2);
    int lineNb = 1;
    bool owner = true;

    fsmEdge* edge = new fsmEdge(sourceNode, targetNode, nullptr, lineNb, owner);

    EXPECT_EQ(edge->parent, sourceNode->getParent());
    EXPECT_EQ(edge->getSourceNode(), sourceNode);
    EXPECT_EQ(edge->getTargetNode(), targetNode);
    EXPECT_EQ(edge->getLineNb(), lineNb);
    EXPECT_EQ(edge->getProbability(), 1.0);
    EXPECT_EQ(edge->owner, owner);

    // Cleanup
    delete edge;
}

TEST_F(FsmEdgeTest, ChangeTargetTest) {
    // Test case: Create an fsmEdge with valid arguments
    fsmNode * sourceNode = myFSM->createFsmNode(0, 1);
    fsmNode * targetNode = myFSM->createFsmNode(1, 2);
    fsmNode * targetNodeAlternative = myFSM->createFsmNode(1, 2);

    int lineNb = 1;
    bool owner = true;

    fsmEdge* edge = new fsmEdge(sourceNode, targetNode, nullptr, lineNb, owner);
    targetNode->addInputTransition(edge);

    EXPECT_EQ(edge->parent, sourceNode->getParent());
    EXPECT_EQ(edge->getSourceNode(), sourceNode);
    EXPECT_EQ(edge->getTargetNode(), targetNode);
    EXPECT_EQ(edge->getLineNb(), lineNb);
    EXPECT_EQ(edge->getProbability(), 1.0);
    EXPECT_EQ(edge->owner, owner);

    // Change the target node
    edge->setTargetNode(targetNodeAlternative);
    EXPECT_EQ(edge->getTargetNode(), targetNodeAlternative);

    // Cleanup
    delete edge;
}

TEST_F(FsmEdgeTest, ChangeSourceTest) {
    // Test case: Create an fsmEdge with valid arguments
    fsmNode * sourceNode = myFSM->createFsmNode(0, 1);
    fsmNode * targetNode = myFSM->createFsmNode(1, 2);
    fsmNode * sourceNodeAlternative = myFSM->createFsmNode(1, 2);

    int lineNb = 1;
    bool owner = true;

    fsmEdge* edge = new fsmEdge(sourceNode, targetNode, nullptr, lineNb, owner);
    targetNode->addInputTransition(edge);
    sourceNode->addTransition(edge);

    EXPECT_EQ(edge->parent, sourceNode->getParent());
    EXPECT_EQ(edge->getSourceNode(), sourceNode);
    EXPECT_EQ(edge->getTargetNode(), targetNode);
    EXPECT_EQ(edge->getLineNb(), lineNb);
    EXPECT_EQ(edge->getProbability(), 1.0);
    EXPECT_EQ(edge->owner, owner);

    // Change the target node
    edge->setSourceNode(sourceNodeAlternative);
    EXPECT_EQ(edge->getSourceNode(), sourceNodeAlternative);

    // Cleanup
    delete edge;
}