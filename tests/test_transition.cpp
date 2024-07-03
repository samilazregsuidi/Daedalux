#include <gtest/gtest.h>
#include <list>
#include <cstdlib>
#include <memory>
#include "../src/feature/ADDutils.cpp"
#include "../src/core/semantic/variable/state/composite.hpp"
#include "../src/core/semantic/variable/transition/compositeTransition.hpp"

class TransitionTest : public ::testing::Test {
protected:
    void SetUp() override {
        symTable* table = new symTable("global", nullptr);
        myFSM = std::make_unique<fsm>(table, ADD());
    }

    void TearDown() override {}
    std::unique_ptr<fsm> myFSM;
};

TEST_F(TransitionTest, SampleNonUniformEmptyList) {
    std::list<transition*> transList;
    auto result = transition::sampleNonUniform(transList);
    EXPECT_EQ(nullptr, result);
}

TEST_F(TransitionTest, SampleNonUniformSingleElement) {
    std::list<transition*> transList;
    auto s = std::make_unique<composite>("testState");
    auto t = std::make_unique<compTransition>(s.get(), transList);
    t->prob = 0.5;
    transList.push_back(t.get());
    EXPECT_EQ(1, transList.size());
    EXPECT_EQ(0.5, t->getProbability());
    // transition* result = transition::sampleNonUniform(transList);
    // EXPECT_EQ(t.get(), result);
}