#include <filesystem>
#include <gtest/gtest.h>
#include <memory>

#include "../../src/core/semantic/semantic.hpp"
#include "../../src/algorithm/explore.hpp"
#include "../../src/core/automata/fsmEdge.hpp"
#include "../../src/core/automata/fsmNode.hpp"

// Define a fixture for the tests
class Executables : public ::testing::Test {
protected:
  void SetUp() override {}

  void TearDown() override
  {
    // Common teardown code that will be called after each test
  }
  std::string helloWorld ="active proctype test(){\n\
                            byte s;\n\
                            do\n\
                            :: s == 0 -> s = 1;\n\
                            :: s == 1 -> s = 2;\n\
                            :: s == 2 -> s = 0;\n\
                            :: s == 2 -> s = 1;\n\
                            od;\n\
                          }";

  std::string helloChanRDV = "chan c = [0] of {byte};\n\
                          active proctype foo(){\n\
                            do\n\
                            :: c!1;\n\
                            :: skip;\n\
                            od;\n\
                          }\n\
                          active proctype bar(){\n\
                            byte x;\n\
                            do\n\
                            :: c?x;\n\
                            od;\n\
                          }";

  std::string helloChanBuf = "chan c = [1] of {byte};\n\
                          active proctype foo(){\n\
                            do\n\
                            :: c!1;\n\
                            :: skip;\n\
                            od;\n\
                          }\n\
                          active proctype bar(){\n\
                            byte x;\n\
                            do\n\
                            :: c?x;\n\
                            :: skip;\n\
                            od;\n\
                          }";

  std::string helloComp =  "active test(){\n\
                              byte s;\n\
                              do\n\
                              :: s = 1;\n\
                              :: s = 2;\n\
                              od;\n\
                            }\n\
                            system s1;\n\
                            system s2;";
};


TEST_F(Executables, simpleExecutables)
{
  const TVL * tvl = nullptr;
  auto original_loader = std::make_unique<promela_loader>(helloWorld, tvl);
  auto originalFSM = original_loader->getAutomata();
  // Create the initial state for both automata
  auto state = initState::createInitState(originalFSM.get(), tvl);

  auto prog = state->getVariables().front();
  ASSERT_EQ(prog->getLocalName(), "");
  ASSERT_EQ(prog, state->getVariable(""));

  process* test = dynamic_cast<process*>(state->getVariable("test"));
  ASSERT_EQ(test, prog->getVariable("test"));
  ASSERT_EQ(state->getValue<primitiveVariable*>("s"), 0);
  ASSERT_EQ(test->getLocation(), 3);

  //get the executables
  auto execs = state->executables();
  // s == 0
  ASSERT_EQ(execs.size(), 1);
  
  auto exec = execs.front();
  ASSERT_EQ(exec->src, state);

  threadTransition* testExec = dynamic_cast<threadTransition*>(exec->getTransition("test"));
  ASSERT_EQ(testExec->src, test);
  ASSERT_EQ(testExec->getType(), astNode::E_EXPR_EQ);
  ASSERT_EQ(testExec->getLineNb(), 4);

  //go the the next state
  state = state->fire(exec);
  execs = state->executables();
  // s = 1
  ASSERT_EQ(execs.size(), 1);

  exec = execs.front();
  ASSERT_EQ(exec->src, state);

  testExec = dynamic_cast<threadTransition*>(exec->getTransition("test"));
  ASSERT_NE(testExec->src, test);

  ASSERT_EQ(testExec->getType(), astNode::E_STMNT_ASGN);

  state = state->fire(exec);
  ASSERT_EQ(state->getValue<primitiveVariable*>("test.s"), 1);
  execs = state->executables();
  // s == 1
  ASSERT_EQ(execs.size(), 1);
  exec = execs.front();
  
  testExec = dynamic_cast<threadTransition*>(exec->getTransition("test"));
  ASSERT_EQ(testExec->getType(), astNode::E_EXPR_EQ);
  
  state = state->fire(exec);
  execs = state->executables();
  // s = 2
  ASSERT_EQ(execs.size(), 1);
  exec = execs.front();

  testExec = dynamic_cast<threadTransition*>(exec->getTransition("test"));
  ASSERT_EQ(testExec->getType(), astNode::E_STMNT_ASGN);
  state = state->fire(exec);
  ASSERT_EQ(state->getValue<primitiveVariable*>("test.s"), 2);

  execs = state->executables();
  // s == 2
  // s == 2
  ASSERT_EQ(execs.size(), 2);
  auto exec0 = execs.front();
  auto exec1 = execs.back();
  ASSERT_EQ(exec0->src, state);
  ASSERT_EQ(exec1->src, state);
  ASSERT_EQ(dynamic_cast<threadTransition*>(exec0->getTransition("test"))-> getLineNb(), 6);
  ASSERT_EQ(dynamic_cast<threadTransition*>(exec1->getTransition("test"))->getLineNb(), 7);

  auto state0 = state->fire(exec0);
  auto state1 = state->fire(exec1);
  ASSERT_NE(state0, state1);

  state0 = state0->Post().front();
  state1 = state1->Post().front();

  ASSERT_EQ(state0->getValue<primitiveVariable*>("test.s"), 0);
  ASSERT_EQ(state1->getValue<primitiveVariable*>("test.s"), 1);
}

TEST_F(Executables, simpleExecutablesChanRDV)
{
  const TVL * tvl = nullptr;
  auto original_loader = std::make_unique<promela_loader>(helloChanRDV, tvl);
  auto originalFSM = original_loader->getAutomata();
  // Create the initial state for both automata
  auto state = initState::createInitState(originalFSM.get(), tvl);

  auto prog = state->getVariables().front();
  ASSERT_EQ(prog->getLocalName(), "");
  ASSERT_EQ(prog, state->getVariable(""));

  process* foo = dynamic_cast<process*>(state->getVariable("foo"));
  ASSERT_EQ(foo, prog->getVariable("foo"));
  ASSERT_EQ(foo->getLocation(), 3);

  process* bar = dynamic_cast<process*>(state->getVariable("bar"));
  ASSERT_EQ(bar, prog->getVariable("bar"));
  ASSERT_EQ(bar->getLocation(), 10);

  //get the executables
  auto execs = state->executables();
  // c!1
  ASSERT_EQ(execs.size(), 2);
  
  auto rdv = execs.front();
  auto skip = execs.back();
  ASSERT_EQ(rdv->src, state);
  ASSERT_EQ(skip->src, state);

  auto fooRDVExec = dynamic_cast<threadTransition*>(rdv->getTransition("foo"));
  ASSERT_EQ(fooRDVExec->src, foo);
  ASSERT_EQ(fooRDVExec->getType(), astNode::E_STMNT_CHAN_SND);
  ASSERT_EQ(fooRDVExec->getLineNb(), 4);

  auto barRDVExec = dynamic_cast<threadTransition*>(rdv->getTransition("bar"));
  ASSERT_EQ(barRDVExec->src, bar);
  ASSERT_EQ(barRDVExec->getType(), astNode::E_STMNT_CHAN_RCV);
  ASSERT_EQ(barRDVExec->getLineNb(), 11);

  ASSERT_EQ(bar->getValue<primitiveVariable*>("x"), 0);

  auto progRDVExec = dynamic_cast<rendezVousTransition*>(rdv->getTransition(""));
  ASSERT_EQ(progRDVExec->src, dynamic_cast<process*>(fooRDVExec->src)->getProgState());
  ASSERT_EQ(progRDVExec, fooRDVExec->parent);
  ASSERT_EQ(progRDVExec->getQuestion(), fooRDVExec);

  progRDVExec->getResponse()->print();

  ASSERT_EQ(progRDVExec->src, dynamic_cast<process*>(barRDVExec->src)->getProgState());
  ASSERT_EQ(progRDVExec, barRDVExec->parent);
  ASSERT_EQ(progRDVExec->getResponse(), barRDVExec);

  //go the the next state
  state = state->fire(rdv);

  foo = dynamic_cast<process*>(state->getVariable("foo"));
  ASSERT_EQ(foo->getLocation(), 3);

  bar = dynamic_cast<process*>(state->getVariable("bar"));
  ASSERT_EQ(bar->getLocation(), 10);

  auto x = state->getValue<primitiveVariable*>("bar.x");
  ASSERT_EQ(bar->getValue<primitiveVariable*>("x"), 1);


}

// TEST_F(Executables, simpleExecutablesChanBuf)
// {
//   const TVL * tvl = nullptr;
//   auto original_loader = std::make_unique<promela_loader>(helloChanBuf, tvl);
//   auto originalFSM = original_loader->getAutomata();
//   // Create the initial state for both automata
//   auto state = initState::createInitState(originalFSM.get(), tvl);

//   auto prog = state->getVariables().front();
//   ASSERT_EQ(prog->getLocalName(), "");
//   ASSERT_EQ(prog, state->getVariable(""));

//   process* foo = dynamic_cast<process*>(state->getVariable("foo"));
//   ASSERT_EQ(foo, prog->getVariable("foo"));
//   ASSERT_EQ(foo->getLocation(), 3);

//   process* bar = dynamic_cast<process*>(state->getVariable("bar"));
//   ASSERT_EQ(bar, prog->getVariable("bar"));
//   ASSERT_EQ(bar->getLocation(), 10);

//   //get the executables
//   auto execs = state->executables();
//   // c!1
//   ASSERT_EQ(execs.size(), 1);
  
//   auto exec = execs.front();
//   ASSERT_EQ(exec->src, state);

//   threadTransition* fooExec = dynamic_cast<threadTransition*>(exec->getTransition("foo"));
//   ASSERT_EQ(fooExec->src, foo);
//   ASSERT_EQ(fooExec->getType(), astNode::E_STMNT_CHAN_SND);
//   ASSERT_EQ(fooExec->getLineNb(), 4);

//   //go the the next state
//   state = state->fire(exec);
//   execs = state->executables();
//   // c?x
//   ASSERT_EQ(execs.size(), 1);

//   exec = execs.front();
//   ASSERT_EQ(exec->src, state);

//   threadTransition* barExec = dynamic_cast<threadTransition*>(exec->getTransition("bar"));
//   ASSERT_EQ(barExec->src, bar);
//   ASSERT_EQ(barExec->getType(), astNode::E_STMNT_CHAN_RCV);
//   ASSERT_EQ(barExec->getLineNb(), 9);

//   state = state->fire(exec);
//   execs = state->executables();
//   // c!1
//   ASSERT_EQ(execs.size(), 1);
//   exec = execs.front();
  
//   fooExec = dynamic_cast<threadTransition*>(exec->getTransition("foo"));
//   ASSERT_EQ(fooExec->getType(), astNode::E_STMNT_CHAN_SND);
// }

// TEST_F(Executables, simpleExecutablesComp)
// {
//   const TVL * tvl = nullptr;
//   auto original_loader = std::make_unique<promela_loader>(helloComp, tvl);
//   auto originalFSM = original_loader->getAutomata();
//   // Create the initial state for both automata
//   auto state = initState::createInitState(originalFSM.get(), tvl);

//   auto prog = state->getVariables().front();
//   ASSERT_EQ(prog->getLocalName(), "");
//   ASSERT_EQ(prog, state->getVariable(""));

//   process* test = dynamic_cast<process*>(state->getVariable("test"));
//   ASSERT_EQ(test, prog->getVariable("test"));
//   ASSERT_EQ(state->getValue<primitiveVariable*>("s"), 0);
//   ASSERT_EQ(test->getLocation(), 3);

//   //get the executables
//   auto execs = state->executables();
//   // s = 1
//   ASSERT_EQ(execs.size(), 1);
  
//   auto exec = execs.front();
//   ASSERT_EQ(exec->src, state);

//   threadTransition* testExec = dynamic_cast<threadTransition*>(exec->getTransition("test"));
//   ASSERT_EQ(testExec->src, test);
//   ASSERT_EQ(testExec->getType(), astNode::E_STMNT_ASGN);
//   ASSERT_EQ(testExec->getLineNb(), 4);

//   //go the the next state
//   state = state->fire(exec);
//   execs = state->executables();
//   // s = 2
//   ASSERT_EQ(execs.size(), 1);

//   exec = execs.front();
//   ASSERT_EQ(exec->src, state);

//   testExec = dynamic_cast<threadTransition*>(exec->getTransition("test"));
//   ASSERT_NE(testExec->src, test);

//   ASSERT_EQ(testExec->getType(), astNode::E_STMNT_ASGN);

//   state = state->fire(exec);
//   ASSERT_EQ(state->getValue<primitiveVariable*>("test.s"), 2);
//   execs = state->executables();
//   // s == 2
//   ASSERT_EQ(execs.size(), 1);
//   exec = execs.front();
  
//   testExec = dynamic_cast<threadTransition*>(exec->getTransition("test"));
//   ASSERT_EQ(testExec->getType(), astNode::E_STMNT_ASGN);
  
//   state = state->fire(exec);
//   ASSERT_EQ(state->getValue<primitiveVariable*>("test.s"), 2);

// }