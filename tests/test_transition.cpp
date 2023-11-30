#include <gtest/gtest.h>
#include <list>
#include <cstdlib>
#include "../src/semantic/compositeState.hpp"
#include "../src/semantic/compositeTransition.hpp"

class TransitionTest : public ::testing::Test {
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

TEST_F(TransitionTest, SampleNonUniformEmptyList) {
    std::list<transition*> transList;
    transition* result = transition::sampleNonUniform(transList);
    EXPECT_EQ(nullptr, result);
}

TEST_F(TransitionTest, SampleNonUniformSingleElement) {
    std::list<transition*> transList = std::list<transition*>();
    auto state = new compState("test_variable");
    transition* t = new compTransition(state, transList);
    t->prob = 0.5;
    transList.push_back(t);
    EXPECT_EQ(1, transList.size());
    EXPECT_EQ(0.5, t->getProbability());
    transition* result = transition::sampleNonUniform(transList);
    EXPECT_EQ(t, result);
}

TEST_F(TransitionTest, SampleNonUniformMultipleElements) {
    std::list<transition*> transList;
    auto s1 = new compState("test_variable");
    transition* t1 = new compTransition(s1, transList);
    t1->prob = 0.3;
    EXPECT_EQ(0.3, t1->getProbability());
    transList.push_back(t1);
    auto s2 = new compState("test_variable");
    transition* t2 = new compTransition(s2, transList);
    t2->prob = 0.6;
    EXPECT_EQ(0.6, t2->getProbability());
    transList.push_back(t2);
    auto s3 = new compState("test_variable");
    transition* t3 = new compTransition(s3, transList);
    t3->prob = 0.1;
    transList.push_back(t3);
    EXPECT_EQ(0.1, t3->getProbability());

    EXPECT_EQ(3, transList.size());

    transition* result = transition::sampleNonUniform(transList);
    EXPECT_TRUE(result == t1 || result == t2 || result == t3);
}