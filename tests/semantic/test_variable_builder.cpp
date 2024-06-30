#include <filesystem>
#include <gtest/gtest.h>
#include <memory>

#include "../../src/core/semantic/semantic.hpp"
#include "../../src/algorithm/explore.hpp"
#include "../../src/core/automata/fsmEdge.hpp"
#include "../../src/core/automata/fsmNode.hpp"

// Define a fixture for the tests
class VariableBuilderTest : public ::testing::Test {
protected:
  void SetUp() override {}

  void TearDown() override
  {
    // Common teardown code that will be called after each test
  }
};

TEST_F(VariableBuilderTest, BasicVariables)
{
  PrimitiveVariable* var = new PrimitiveVariable("i", symbol::T_INT);
  var->setInitValue(1);
  PrimitiveVariable* var = new PrimitiveVariable("j", symbol::T_INT);

}

// TEST_F(stateInit, InitStateHelloWorldChan)
// {
//   const TVL * tvl = nullptr;
//   auto original_loader = std::make_unique<promela_loader>(helloChan, tvl);
//   auto originalFSM = original_loader->getAutomata();
//   // Create the initial state for both automata
//   auto state = initState::createInitState(originalFSM.get(), tvl);

//   auto c = state->getVariable("c");
//   ASSERT_EQ(c->getValue<primitiveVariable*>("c[0]"), 0);
//   ASSERT_EQ(dynamic_cast<channel*>(c)->len(), 1);

//   auto d = state->getVariable("d");
//   ASSERT_EQ(d->getValue<boolVar*>("d[0]"), false);
//   ASSERT_EQ(dynamic_cast<channel*>(d)->len(), 1);

//   auto cd = state->getVariable("cd");
//   ASSERT_EQ(cd->getValue<primitiveVariable*>("cd[0]"), 0);
//   ASSERT_EQ(cd->getValue<boolVar*>("cd[1]"), false);
//   ASSERT_EQ(dynamic_cast<channel*>(cd)->len(), 2);
// }

