#include <gtest/gtest.h>

#include <memory>   // std::unique_ptr
#include "../src/core/automata/fsm.hpp"
#include "../src/core/automata/fsmEdge.cpp"


class FsmEdgeTest : public ::testing::Test {
protected:
    void SetUp() override {
        symTable* table = new symTable("global", nullptr);
        myFSM = std::make_unique<fsm>(table, ADD());
    }

    void TearDown() override {
    }

    std::unique_ptr<fsm> myFSM;
};

TEST_F(FsmEdgeTest, ConstructorTest) {
    // Test case: Create an fsmEdge with valid arguments
    fsmNode * sourceNode = myFSM->createFsmNode(0, 1);
    fsmNode * targetNode = myFSM->createFsmNode(1, 2);
    int lineNb = 1;
    bool owner = true;

    std::unique_ptr<fsmEdge> edge = std::make_unique<fsmEdge>(sourceNode, targetNode, nullptr, lineNb, owner);

    EXPECT_EQ(edge->parent, sourceNode->getParent());
    EXPECT_EQ(edge->getSourceNode(), sourceNode);
    EXPECT_EQ(edge->getTargetNode(), targetNode);
    EXPECT_EQ(edge->getLineNb(), lineNb);
    EXPECT_EQ(edge->getProbability(), 1.0);
    EXPECT_EQ(edge->owner, owner);
}

TEST_F(FsmEdgeTest, ChangeTargetTest) {
    // Test case: Create an fsmEdge with valid arguments
    fsmNode * sourceNode = myFSM->createFsmNode(0, 1);
    fsmNode * targetNode = myFSM->createFsmNode(1, 2);
    fsmNode * targetNodeAlternative = myFSM->createFsmNode(1, 2);

    int lineNb = 1;
    bool owner = true;

    std::unique_ptr<fsmEdge> edge = std::make_unique<fsmEdge>(sourceNode, targetNode, nullptr, lineNb, owner);
    targetNode->addInputTransition(edge.get());

    EXPECT_EQ(edge->parent, sourceNode->getParent());
    EXPECT_EQ(edge->getSourceNode(), sourceNode);
    EXPECT_EQ(edge->getTargetNode(), targetNode);
    EXPECT_EQ(edge->getLineNb(), lineNb);
    EXPECT_EQ(edge->getProbability(), 1.0);
    EXPECT_EQ(edge->owner, owner);

    // Change the target node
    edge->setTargetNode(targetNodeAlternative);
    EXPECT_EQ(edge->getTargetNode(), targetNodeAlternative);
}

TEST_F(FsmEdgeTest, ChangeSourceTest) {
    // Test case: Create an fsmEdge with valid arguments
    fsmNode * sourceNode = myFSM->createFsmNode(0, 1);
    fsmNode * targetNode = myFSM->createFsmNode(1, 2);
    fsmNode * sourceNodeAlternative = myFSM->createFsmNode(1, 2);

    int lineNb = 1;
    bool owner = true;

    std::unique_ptr<fsmEdge> edge = std::make_unique<fsmEdge>(sourceNode, targetNode, nullptr, lineNb, owner);
    targetNode->addInputTransition(edge.get());
    sourceNode->addTransition(edge.get());

    EXPECT_EQ(edge->parent, sourceNode->getParent());
    EXPECT_EQ(edge->getSourceNode(), sourceNode);
    EXPECT_EQ(edge->getTargetNode(), targetNode);
    EXPECT_EQ(edge->getLineNb(), lineNb);
    EXPECT_EQ(edge->getProbability(), 1.0);
    EXPECT_EQ(edge->owner, owner);

    // Change the target node
    edge->setSourceNode(sourceNodeAlternative);
    EXPECT_EQ(edge->getSourceNode(), sourceNodeAlternative);
}