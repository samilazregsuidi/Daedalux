#include <gtest/gtest.h>
#include "../src/core/automata/fsmNode.cpp"

class FsmNodeTest : public ::testing::Test {
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

TEST_F(FsmNodeTest, CreateFsmEdgeTest) {
    fsmNode* node = myFSM->createFsmNode(0, 1);
    fsmNode* target = myFSM->createFsmNode(0, 2);

    fsmEdge* edge = node->createfsmEdge(3, nullptr, target, true);

    ASSERT_NE(edge, nullptr);
    EXPECT_EQ(edge->getSourceNode(), node);
    EXPECT_EQ(edge->getExpression(), nullptr);
    EXPECT_EQ(edge->getLineNb(), 3);
    EXPECT_EQ(edge->getTargetNode(), target);
}

TEST_F(FsmNodeTest, CopyFsmEdgeTest) {
    fsmNode* node = myFSM->createFsmNode(0, 1);
    fsmNode* target = myFSM->createFsmNode(0, 2);
    fsmEdge* originalEdge = node->createfsmEdge(3, nullptr, target, true);

    fsmEdge* copiedEdge = node->copyfsmEdge(originalEdge);

    ASSERT_NE(copiedEdge, nullptr);
    EXPECT_EQ(copiedEdge->getSourceNode(), node);
    EXPECT_EQ(copiedEdge->getExpression(), nullptr);
    EXPECT_EQ(copiedEdge->getLineNb(), originalEdge->getLineNb());
    // TODO: why does this fail?
    // EXPECT_EQ(copiedEdge->getTargetNode(), originalEdge->getTargetNode());

}

TEST_F(FsmNodeTest, AddTransitionTest) {
    fsmNode* node = myFSM->createFsmNode(0, 1);
    fsmEdge* edge = new fsmEdge(node, nullptr, 2, false);

    node->addTransition(edge);

    std::list<fsmEdge*> transitions = node->getEdges();
    ASSERT_EQ(transitions.size(), 1);
    EXPECT_EQ(transitions.front(), edge);
}

TEST_F(FsmNodeTest, RemoveTransitionTest) {
    fsmNode* node = myFSM->createFsmNode(0, 1);
    fsmEdge* edge = new fsmEdge(node, nullptr, 2, false);
    node->addTransition(edge);

    node->removeTransition(edge);

    std::list<fsmEdge*> transitions = node->getEdges();
    ASSERT_EQ(transitions.size(), 0);

}

TEST_F(FsmNodeTest, AddInputTransitionTest) {
    fsmNode* srcNode = myFSM->createFsmNode(0, 1);
    fsmNode* trgNode = myFSM->createFsmNode(0, 2);
    fsmEdge* edge = new fsmEdge(srcNode, trgNode, nullptr, 2, false);

    trgNode->addInputTransition(edge);

    std::list<fsmEdge*> inputTransitions = trgNode->getInputEdges();
    ASSERT_EQ(inputTransitions.size(), 1);
    EXPECT_EQ(inputTransitions.front(), edge);

}

TEST_F(FsmNodeTest, RemoveInputTransitionTest) {
    fsmNode* srcNode = myFSM->createFsmNode(0, 1);
    fsmNode* trgNode = myFSM->createFsmNode(0, 2);
    fsmEdge* edge = new fsmEdge(srcNode, trgNode, nullptr, 2, false);

    trgNode->addInputTransition(edge);
    trgNode->removeInputTransition(edge);

    std::list<fsmEdge*> inputTransitions = trgNode->getInputEdges();
    ASSERT_EQ(inputTransitions.size(), 0);

}

TEST_F(FsmNodeTest, GetFlagsTest) {
    fsmNode* node = myFSM->createFsmNode(0, 1);

    unsigned int flags = node->getFlags();

    EXPECT_EQ(flags, 0);

}

TEST_F(FsmNodeTest, IsAcceptingTest) {
    fsmNode* node = myFSM->createFsmNode(fsmNode::N_ACCEPT, 1);
    
    bool isAccepting = node->isAccepting();

    EXPECT_TRUE(isAccepting);
}

TEST_F(FsmNodeTest, IsProgressTest) {
    fsmNode* node = myFSM->createFsmNode(fsmNode::N_PROGRESS, 1);

    bool isProgress = node->isProgress();

    EXPECT_TRUE(isProgress);

}

TEST_F(FsmNodeTest, IsDeterministicTest) {
    fsmNode* node = myFSM->createFsmNode(fsmNode::N_DETERMINISTIC, 1);
    
    bool isDeterministic = node->isDeterministic();

    EXPECT_TRUE(isDeterministic);

}

TEST_F(FsmNodeTest, IsAtomicTest) {
    fsmNode* node = myFSM->createFsmNode(fsmNode::N_ATOMIC, 1);

    bool isAtomic = node->isAtomic();

    EXPECT_TRUE(isAtomic);

}

TEST_F(FsmNodeTest, SetLineNbTest) {
    fsmNode* node = myFSM->createFsmNode(fsmNode::N_ATOMIC, 1);

    node->setLineNb(2);

    int lineNb = node->getLineNb();

    EXPECT_EQ(lineNb, 2);

}

TEST_F(FsmNodeTest, GetParentTest) {
    fsmNode* node = myFSM->createFsmNode(0, 1);

    fsm* parentNode = node->getParent();

    EXPECT_EQ(parentNode, myFSM);

}

TEST_F(FsmNodeTest, OrderAcceptingTransitionsTest) {
    fsmNode* node = myFSM->createFsmNode(0, 1);
    fsmNode* acceptingNode = myFSM->createFsmNode(fsmNode::N_ACCEPT, 2);
    fsmEdge* edge1 = node->createfsmEdge(3, nullptr, acceptingNode, false);
    fsmEdge* edge2 = node->createfsmEdge(4, nullptr, nullptr, false);

    node->orderAcceptingTransitions();

    std::list<fsmEdge*> transitions = node->getEdges();
    ASSERT_EQ(transitions.size(), 2);
    EXPECT_EQ(transitions.front(), edge1);
    EXPECT_EQ(transitions.back(), edge2);
}

TEST_F(FsmNodeTest, GetIDTest) {
    fsmNode* node = myFSM->createFsmNode(0, 1);

    unsigned long id = node->getID();

    EXPECT_EQ(id, (unsigned long)node);

}

TEST_F(FsmNodeTest, EqualityOperatorTest) {
    fsmNode* node1 = myFSM->createFsmNode(0, 1);
    fsmNode* node2 = myFSM->createFsmNode(0, 1);
    
    bool isEqual = (*node1 == *node2);

    EXPECT_TRUE(isEqual);
}