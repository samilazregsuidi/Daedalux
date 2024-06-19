#include <filesystem>
#include <gtest/gtest.h>
#include <memory>

#include "../../src/core/semantic/semantic.hpp"
#include "../../src/algorithm/explore.hpp"
#include "../../src/core/automata/fsmEdge.hpp"
#include "../../src/core/automata/fsmNode.hpp"
#include "../../src/promela/parser/promela_loader.hpp"

// Define a fixture for the tests
class stateApply : public ::testing::Test {
protected:
  void SetUp() override {}

  void TearDown() override
  {
    // Common teardown code that will be called after each test
  }
  std::string helloWorld = "typedef S { int i; bool b = true };\n\
                    S s;\n\
                    active proctype test(){\n\
                     int i = 1; bool b; byte y[2]\n\
                     S s; s.i = 1;\n\
                    }";

  std::string helloChan = "chan c = [0] of {byte};\n\
                          chan d = [0] of {bool};\n\
                          chan cd = [0] of {byte, bool};\n\
                    active proctype test(){\n\
                     int i = 1; bool b;\n\
                     c!1; d!true; cd!1, true;\n\
                    }";

  std::string complexStruct;
};


TEST_F(stateApply, InitStateHelloWorld)
{
  const TVL * tvl = nullptr;
  auto original_loader = std::make_unique<promela_loader>(helloWorld, tvl);
  auto originalFSM = original_loader->getAutomata();
  // Create the initial state for both automata
  auto state = initState::createInitState(originalFSM.get(), tvl);

  auto s_global = state->get<structVar*>("s");

  ASSERT_EQ(state->getValue<intVar*>("s.i"), 0);
  ASSERT_EQ(state->getValue<boolVar*>("s.b"), true);
  ASSERT_EQ(s_global->getValue<intVar*>("i"), 0);
  ASSERT_EQ(s_global->getValue<boolVar*>("b"), true);

  ASSERT_EQ(state->getValue<intVar*>("test.i"), 1);
  ASSERT_EQ(state->getValue<boolVar*>("test.b"), false);
  ASSERT_EQ(state->getValue<intVar*>("test.s.i"), 0);
  ASSERT_EQ(state->getValue<boolVar*>("test.s.b"), true);
  ASSERT_EQ(state->getValue<byteVar*>("test.y[0]"), 0);
  ASSERT_EQ(state->getValue<byteVar*>("test.y[1]"), 0);

  auto proc = state->get<process*>("test");
  ASSERT_EQ(dynamic_cast<process*>(proc)->getLocation(), 5);

  ASSERT_EQ(proc->getValue<intVar*>("i"), 1);
  ASSERT_EQ(proc->getValue<boolVar*>("b"), false);
  ASSERT_EQ(proc->getValue<intVar*>("s.i"), 0);
  ASSERT_EQ(proc->getValue<boolVar*>("s.b"), true);

  auto s_local = proc->get<structVar*>("s");
  ASSERT_NE(s_global, s_local);
  ASSERT_EQ(s_local->getValue<intVar*>("i"), 0);
  ASSERT_EQ(s_local->getValue<boolVar*>("b"), true);
}

TEST_F(stateApply, InitStateHelloWorldChan)
{
  const TVL * tvl = nullptr;
  auto original_loader = std::make_unique<promela_loader>(helloChan, tvl);
  auto originalFSM = original_loader->getAutomata();
  // Create the initial state for both automata
  auto state = initState::createInitState(originalFSM.get(), tvl);

  auto c = state->get("c[0]");
  ASSERT_EQ(c->getValue<byteVar*>(0), 0);
  ASSERT_EQ(state->get<channel*>("c")->len(), 0);

  auto d = state->get("d[0]");
  ASSERT_EQ(d->getValue<boolVar*>(0), false);
  ASSERT_EQ(state->get<channel*>("d")->len(), 0);

  auto cd = state->get("cd[0]");
  ASSERT_EQ(cd->getValue<byteVar*>(0), 0);
  ASSERT_EQ(cd->getValue<boolVar*>(1), false);
  ASSERT_EQ(state->get<channel*>("cd")->len(), 0);
}

