#include <filesystem>
#include <gtest/gtest.h>
#include <memory>

#include "../../src/core/semantic/semantic.hpp"
#include "../../src/algorithm/explore.hpp"
#include "../../src/core/automata/fsmEdge.hpp"
#include "../../src/core/automata/fsmNode.hpp"
#include "../../src/promela/parser/promela_loader.hpp"

// Define a fixture for the tests
class ExecutableTests : public ::testing::Test {
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
                            od;\n\
                          }\n\
                          active proctype bar(){\n\
                            byte x;\n\
                            do\n\
                            :: c?x;\n\
                            od;\n\
                          }";

  std::string helloChanRDV2 = "chan c = [0] of {byte, byte};\n\
                          active proctype foo(){\n\
                            do\n\
                            :: c!1,2;\n\
                            od;\n\
                          }\n\
                          active proctype bar(){\n\
                            byte x,y;\n\
                            do\n\
                            :: c?x,y;\n\
                            od;\n\
                          }";

  std::string helloChanRDV3 = "chan c = [0] of {byte, byte};\n\
                          active proctype foo(){\n\
                            byte z = 2;\n\
                            do\n\
                            :: c!1,z; z = z + 1;\n\
                            od;\n\
                          }\n\
                          active proctype bar(){\n\
                            byte x,y;\n\
                            do\n\
                            :: c?x,y;\n\
                            od;\n\
                          }";


  std::string helloChanRDVisRecv = "chan c = [0] of {byte};\n\
                          active proctype foo(){\n\
                            do\n\
                            :: c!1;\n\
                            od;\n\
                          }\n\
                          active proctype bar(){\n\
                            do\n\
                            :: c?1;\n\
                            od;\n\
                          }";

  std::string helloChanRDVisRecv2 = "chan c = [0] of {byte};\n\
                          active proctype foo(){\n\
                            do\n\
                            :: c!1;\n\
                            :: c!2;\n\
                            od;\n\
                          }\n\
                          active proctype bar(){\n\
                            byte x;\n\
                            do\n\
                            :: c?2;\n\
                            :: c?1;\n\
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

  std::string helloComp =  "active proctype test(){\n\
                              byte s;\n\
                              do\n\
                              :: s = 255;\n\
                              :: s = 1;\n\
                              od;\n\
                            }\n\
                            system s1;\n\
                            system s2;";

  std::string helloChanAtomic = "mtype = {na, water, methane, user} \n\
                                chan lvl = [0] of {mtype};\n\
                                chan sensor = [0] of {mtype};\n\
                                chan cmd = [0] of {mtype};\n\
                                mtype msg = na;\n\
                                active proctype ctrl(){\n\
                                  mtype x;\n\
                                  do\n\
                                  :: atomic{lvl?x; msg = water;}\n\
                                  :: atomic{sensor?x; msg = methane;}\n\
                                  :: atomic{cmd?x; msg = user;}\n\
                                  od;\n\
                                }\n\
                                active proctype waterProc(){\n\
                                  byte c;\n\
                                  do\n\
                                  :: lvl!na\n\
                                  :: c++;\n\
                                  od;\n\
                                }\n\
                                active proctype methaneProc(){\n\
                                  byte c;\n\
                                  do\n\
                                  :: sensor!na\n\
                                  :: c++;\n\
                                  od;\n\
                                }\n\
                                active proctype userProc(){\n\
                                  byte c;\n\
                                  do\n\
                                  :: cmd!na\n\
                                  :: c++;\n\
                                  od;\n\
                                }\n\
                                ";
};
/*
TEST_F(ExecutableTests, simpleExecutables)
{
  //parsing the promela and tvl files
  auto loader = new promela_loader(promela_file, tvl_file);
  
  //getting the program graph from the parser
  auto program_graph = loader->getAutomata();

  //create the initial state using semantic engine
  auto state = initState::createInitState(program_graph);
  
  //exploring the state space
  auto execs = state->executables();
  assert(execs.size() == 1);
  state->fire(execs.front());

  //checking the safe variable state
  auto safe_var = state->get<boolVar>("safe");
  assert(safe_var->value() == true);

  //checking the motor process location
  auto motor_proc = state->get<process>("motor");
  assert(motor_proc->location() == 3);
  state->fire(state->executables().front());
  assert(motor_proc->location() == 4);

  //checking the state properties
  assert(state->isAccepting() == false);
  assert(state->isDeadlock() == false);

  auto next_states = state->Post();
  for(auto next_state : next_states)
    assert(next_state->get<boolVar>("danger")->value() == false);
}
*/

TEST_F(ExecutableTests, simpleExecutables)
{
  const TVL * tvl = nullptr;

  auto original_loader = std::make_unique<promela_loader>(helloWorld, tvl);
  auto originalFSM = original_loader->getAutomata();
  // Create the initial state for both automata
  auto state = initState::createInitState(originalFSM.get(), tvl);

  auto prog = state->getVariables().front();
  ASSERT_EQ(prog->getLocalName(), "");
  ASSERT_EQ(prog, state->get(""));

  process* test = state->get<process*>("test");
  ASSERT_EQ(test, prog->get("test"));
  ASSERT_EQ(state->getValue<byteVar*>("s"), 0);
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
  ASSERT_EQ(state->getValue<byteVar*>("test.s"), 1);
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
  ASSERT_EQ(state->getValue<byteVar*>("test.s"), 2);

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

  ASSERT_EQ(state0->getValue<byteVar*>("test.s"), 0);
  ASSERT_EQ(state1->getValue<byteVar*>("test.s"), 1);
}

TEST_F(ExecutableTests, simpleExecutablesChanRDV)
{
  const TVL * tvl = nullptr;
  auto original_loader = std::make_unique<promela_loader>(helloChanRDV, tvl);
  auto originalFSM = original_loader->getAutomata();
  // Create the initial state for both automata
  auto state = initState::createInitState(originalFSM.get(), tvl);

  auto prog = state->getVariables().front();
  ASSERT_EQ(prog->getLocalName(), "");
  ASSERT_EQ(prog, state->get(""));

  process* foo = state->get<process*>("foo");
  ASSERT_EQ(foo, prog->get("foo"));
  ASSERT_EQ(foo->getLocation(), 3);

  process* bar = state->get<process*>("bar");
  ASSERT_EQ(bar, prog->get("bar"));
  ASSERT_EQ(bar->getLocation(), 9);

  //get the executables
  auto execs = state->executables();
  // c!1
  ASSERT_EQ(execs.size(), 1);
  
  auto rdv = execs.front();
  ASSERT_EQ(rdv->src, state);

  auto fooRDVExec = dynamic_cast<threadTransition*>(rdv->getTransition("foo"));
  ASSERT_EQ(fooRDVExec->src, foo);
  ASSERT_EQ(fooRDVExec->getType(), astNode::E_STMNT_CHAN_SND);
  ASSERT_EQ(fooRDVExec->getLineNb(), 4);

  auto barRDVExec = dynamic_cast<threadTransition*>(rdv->getTransition("bar"));
  ASSERT_EQ(barRDVExec->src, bar);
  ASSERT_EQ(barRDVExec->getType(), astNode::E_STMNT_CHAN_RCV);
  ASSERT_EQ(barRDVExec->getLineNb(), 10);

  ASSERT_EQ(bar->getValue<byteVar*>("x"), 0);

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

  foo = state->get<process*>("foo");
  ASSERT_EQ(foo->getLocation(), 3);

  bar = state->get<process*>("bar");
  ASSERT_EQ(bar->getLocation(), 9);

  ASSERT_EQ(bar->getValue<byteVar*>("x"), 1);

  execs = state->executables();
  assert(execs.size() == 1);
  rdv = execs.front(); 
  fooRDVExec = dynamic_cast<threadTransition*>(rdv->getTransition("foo"));
  ASSERT_EQ(fooRDVExec->src, foo);
  ASSERT_EQ(fooRDVExec->getType(), astNode::E_STMNT_CHAN_SND);
  ASSERT_EQ(fooRDVExec->getLineNb(), 4);
  
  barRDVExec = dynamic_cast<threadTransition*>(rdv->getTransition("bar"));
  ASSERT_EQ(barRDVExec->src, bar);
  ASSERT_EQ(barRDVExec->getType(), astNode::E_STMNT_CHAN_RCV);
  ASSERT_EQ(barRDVExec->getLineNb(), 10);
  
  state = state->fire(rdv);

  foo = state->get<process*>("foo");
  ASSERT_EQ(foo->getLocation(), 3);

  bar = state->get<process*>("bar");
  ASSERT_EQ(bar->getLocation(), 9);

  ASSERT_EQ(bar->getValue<byteVar*>("x"), 1);
}

TEST_F(ExecutableTests, simpleExecutablesChanRDV2)
{
  const TVL * tvl = nullptr;
  auto original_loader = std::make_unique<promela_loader>(helloChanRDV2, tvl);
  auto originalFSM = original_loader->getAutomata();
  // Create the initial state for both automata
  auto state = initState::createInitState(originalFSM.get(), tvl);

  auto prog = state->getVariables().front();
  ASSERT_EQ(prog->getLocalName(), "");
  ASSERT_EQ(prog, state->get(""));

  process* foo = state->get<process*>("foo");
  ASSERT_EQ(foo, prog->get("foo"));
  ASSERT_EQ(foo->getLocation(), 3);

  process* bar = state->get<process*>("bar");
  ASSERT_EQ(bar, prog->get("bar"));
  ASSERT_EQ(bar->getLocation(), 9);

  //get the executables
  auto execs = state->executables();
  // c!1
  ASSERT_EQ(execs.size(), 1);
  
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
  ASSERT_EQ(barRDVExec->getLineNb(), 10);

  ASSERT_EQ(bar->getValue<byteVar*>("x"), 0);
  ASSERT_EQ(bar->getValue<byteVar*>("y"), 0);

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

  foo = state->get<process*>("foo");
  ASSERT_EQ(foo->getLocation(), 3);

  bar = state->get<process*>("bar");
  ASSERT_EQ(bar->getLocation(), 9);

  ASSERT_EQ(bar->getValue<byteVar*>("x"), 1);
  ASSERT_EQ(bar->getValue<byteVar*>("y"), 2);
}

TEST_F(ExecutableTests, simpleExecutablesChanRDV3)
{
  const TVL * tvl = nullptr;
  auto original_loader = std::make_unique<promela_loader>(helloChanRDV3, tvl);
  auto originalFSM = original_loader->getAutomata();
  // Create the initial state for both automata
  auto state = initState::createInitState(originalFSM.get(), tvl);

  auto prog = state->getVariables().front();
  ASSERT_EQ(prog->getLocalName(), "");
  ASSERT_EQ(prog, state->get(""));

  process* foo = state->get<process*>("foo");
  ASSERT_EQ(foo, prog->get("foo"));
  ASSERT_EQ(foo->getLocation(), 4);

  process* bar = state->get<process*>("bar");
  ASSERT_EQ(bar, prog->get("bar"));
  ASSERT_EQ(bar->getLocation(), 10);

  //get the executables
  auto execs = state->executables();
  // c!1
  ASSERT_EQ(execs.size(), 1);
  
  auto rdv = execs.front();
  auto skip = execs.back();
  ASSERT_EQ(rdv->src, state);
  ASSERT_EQ(skip->src, state);

  auto fooRDVExec = dynamic_cast<threadTransition*>(rdv->getTransition("foo"));
  ASSERT_EQ(fooRDVExec->src, foo);
  ASSERT_EQ(fooRDVExec->getType(), astNode::E_STMNT_CHAN_SND);
  ASSERT_EQ(fooRDVExec->getLineNb(), 5);

  auto barRDVExec = dynamic_cast<threadTransition*>(rdv->getTransition("bar"));
  ASSERT_EQ(barRDVExec->src, bar);
  ASSERT_EQ(barRDVExec->getType(), astNode::E_STMNT_CHAN_RCV);
  ASSERT_EQ(barRDVExec->getLineNb(), 11);

  ASSERT_EQ(bar->getValue<byteVar*>("x"), 0);
  ASSERT_EQ(bar->getValue<byteVar*>("y"), 0);

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

  foo = state->get<process*>("foo");
  ASSERT_EQ(foo->getLocation(), 5);

  bar = state->get<process*>("bar");
  ASSERT_EQ(bar->getLocation(), 10);

  ASSERT_EQ(bar->getValue<byteVar*>("x"), 1);
  ASSERT_EQ(bar->getValue<byteVar*>("y"), 2);

  auto post = state->Post();
  assert(post.size() == 1);
  state = post.front();

  foo = state->get<process*>("foo");
  ASSERT_EQ(foo->getLocation(), 4);
  ASSERT_EQ(foo->getValue<byteVar*>("z"), 3);
  bar = state->get<process*>("bar");
  ASSERT_EQ(bar->getLocation(), 10);
  ASSERT_EQ(bar->getValue<byteVar*>("y"), 2);

  state = state->Post().front();
  foo = state->get<process*>("foo");
  ASSERT_EQ(foo->getLocation(), 5);
  ASSERT_EQ(foo->getValue<byteVar*>("z"), 3);
  bar = state->get<process*>("bar");
  ASSERT_EQ(bar->getLocation(), 10);
  ASSERT_EQ(bar->getValue<byteVar*>("y"), 3);

  state = state->Post().front();
  foo = state->get<process*>("foo");
  ASSERT_EQ(foo->getLocation(), 4);
  ASSERT_EQ(foo->getValue<byteVar*>("z"), 4);
  bar = state->get<process*>("bar");
  ASSERT_EQ(bar->getLocation(), 10);
  ASSERT_EQ(bar->getValue<byteVar*>("y"), 3);

  state = state->Post().front();
  foo = state->get<process*>("foo");
  ASSERT_EQ(foo->getLocation(), 5);
  ASSERT_EQ(foo->getValue<byteVar*>("z"), 4);
  bar = state->get<process*>("bar");
  ASSERT_EQ(bar->getLocation(), 10);
  ASSERT_EQ(bar->getValue<byteVar*>("y"), 4);
}

TEST_F(ExecutableTests, simpleExecutablesChanRDVatomic)
{
  const TVL * tvl = nullptr;
  auto original_loader = std::make_unique<promela_loader>(helloChanAtomic, tvl);

  auto originalFSM = original_loader->getAutomata();
  // Create the initial state for both automata
  auto state = initState::createInitState(originalFSM.get(), tvl);

  auto prog = state->getVariables().front();
  ASSERT_EQ(prog->getLocalName(), "");
  ASSERT_EQ(prog, state->get(""));

  process* ctrl = state->get<process*>("ctrl");
  ASSERT_EQ(ctrl, prog->get("ctrl"));
  ASSERT_EQ(ctrl->getLocation(), 8);

  process* waterProc = state->get<process*>("waterProc");
  ASSERT_EQ(waterProc, prog->get("waterProc"));
  ASSERT_EQ(waterProc->getLocation(), 16);

  process* methaneProc = state->get<process*>("methaneProc");
  ASSERT_EQ(methaneProc, prog->get("methaneProc"));
  ASSERT_EQ(methaneProc->getLocation(), 23);

  process* userProc = state->get<process*>("userProc");
  ASSERT_EQ(userProc, prog->get("userProc"));
  ASSERT_EQ(userProc->getLocation(), 30);

  //get the executables
  auto execs = state->executables();
  // atomic{lvl?x; msg = water;}
  ASSERT_EQ(execs.size(), 6);

  transition* toFire = nullptr;
  for(auto exec : execs)
  {
    ASSERT_EQ(exec->src, state);
    auto ctrlExec = dynamic_cast<threadTransition*>(exec->getTransition("ctrl"));
    ASSERT_EQ(ctrlExec->src, ctrl);
    if(ctrlExec->getType(), astNode::E_STMNT_CHAN_RCV);
    {
      toFire = exec;
      break;
    }
  }

  ASSERT_NE(toFire, nullptr);
  state = state->fire(toFire);
  ctrl = state->get<process*>("ctrl");
  ASSERT_EQ(ctrl->getLocation(), 9);
  ASSERT_TRUE(*state->get<mtypeVar*>("msg") == "na");

  execs = state->executables();
  // atomic{lvl?x; msg = water;}
  ASSERT_EQ(execs.size(), 1);

  //assumption, this is the first recv
  state = state->fire(execs.front());
  ctrl = state->get<process*>("ctrl");
  ASSERT_EQ(ctrl->getLocation(), 8);
  ASSERT_TRUE(*state->get<mtypeVar*>("msg") == "water");
}

// TEST_F(ExecutableTests, simpleExecutablesChanRDVisRecv)
// {
//   const TVL * tvl = nullptr;
//   auto original_loader = std::make_unique<promela_loader>(helloChanRDVisRecv, tvl);
//   auto originalFSM = original_loader->getAutomata();
//   // Create the initial state for both automata
//   auto state = initState::createInitState(originalFSM.get(), tvl);

//   auto prog = state->getVariables().front();
//   ASSERT_EQ(prog->getLocalName(), "");
//   ASSERT_EQ(prog, state->get(""));

//   process* foo = state->get<process*>("foo");
//   ASSERT_EQ(foo, prog->get("foo"));
//   ASSERT_EQ(foo->getLocation(), 3);

//   process* bar = state->get<process*>("bar");
//   ASSERT_EQ(bar, prog->get("bar"));
//   ASSERT_EQ(bar->getLocation(), 8);

//   //get the executables
//   auto execs = state->executables();
//   // c!1
//   ASSERT_EQ(execs.size(), 1);
  
//   auto rdv = execs.front();
//   auto skip = execs.back();
//   ASSERT_EQ(rdv->src, state);
//   ASSERT_EQ(skip->src, state);

//   auto fooRDVExec = dynamic_cast<threadTransition*>(rdv->getTransition("foo"));
//   ASSERT_EQ(fooRDVExec->src, foo);
//   ASSERT_EQ(fooRDVExec->getType(), astNode::E_STMNT_CHAN_SND);
//   ASSERT_EQ(fooRDVExec->getLineNb(), 4);

//   auto barRDVExec = dynamic_cast<threadTransition*>(rdv->getTransition("bar"));
//   ASSERT_EQ(barRDVExec->src, bar);
//   ASSERT_EQ(barRDVExec->getType(), astNode::E_STMNT_CHAN_RCV);
//   ASSERT_EQ(barRDVExec->getLineNb(), 9);

//   auto progRDVExec = dynamic_cast<rendezVousTransition*>(rdv->getTransition(""));
//   ASSERT_EQ(progRDVExec->src, dynamic_cast<process*>(fooRDVExec->src)->getProgState());
//   ASSERT_EQ(progRDVExec, fooRDVExec->parent);
//   ASSERT_EQ(progRDVExec->getQuestion(), fooRDVExec);

//   progRDVExec->getResponse()->print();

//   ASSERT_EQ(progRDVExec->src, dynamic_cast<process*>(barRDVExec->src)->getProgState());
//   ASSERT_EQ(progRDVExec, barRDVExec->parent);
//   ASSERT_EQ(progRDVExec->getResponse(), barRDVExec);

//   //go the the next state
//   state = state->fire(rdv);

//   foo = state->get<process*>("foo");
//   ASSERT_EQ(foo->getLocation(), 3);

//   bar = state->get<process*>("bar");
//   ASSERT_EQ(bar->getLocation(), 8);
// }

// TEST_F(ExecutableTests, simpleExecutablesChanBuf)
// {
//   const TVL * tvl = nullptr;
//   auto original_loader = std::make_unique<promela_loader>(helloChanBuf, tvl);
//   auto originalFSM = original_loader->getAutomata();
//   // Create the initial state for both automata
//   auto state = initState::createInitState(originalFSM.get(), tvl);

//   auto prog = state->getVariables().front();
//   ASSERT_EQ(prog->getLocalName(), "");
//   ASSERT_EQ(prog, state->get(""));

//   process* foo = dynamic_cast<process*>(state->get("foo"));
//   ASSERT_EQ(foo, prog->get("foo"));
//   ASSERT_EQ(foo->getLocation(), 3);

//   process* bar = dynamic_cast<process*>(state->get("bar"));
//   ASSERT_EQ(bar, prog->get("bar"));
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


TEST_F(ExecutableTests, simpleExecutablesComp)
{
  const TVL * tvl = nullptr;
  auto original_loader = std::make_unique<promela_loader>(helloComp, tvl);
  auto originalFSM = original_loader->getAutomata();
  // Create the initial state for both automata
  auto state = initState::createInitState(originalFSM.get(), tvl);

  auto prog1 = state->getVariables().front();
  ASSERT_EQ(prog1->getLocalName(), "s1");
  ASSERT_EQ(prog1, state->get("s1"));

  process* test = state->get<process*>("s1.test");
  ASSERT_EQ(test, prog1->get("test"));
  ASSERT_EQ(state->get("s1.test.s"), test->get("s"));
  ASSERT_EQ(state->getValue<byteVar*>("s1.test.s"), 0);
  ASSERT_EQ(test->getLocation(), 3);

  auto prog2 = state->getVariables().back();
  ASSERT_EQ(prog2->getLocalName(), "s2");
  ASSERT_EQ(prog2, state->get("s2"));

  test = state->get<process*>("s2.test");
  ASSERT_EQ(test, prog2->get("test"));
  ASSERT_EQ(state->get<byteVar*>("s2.test.s"), test->get<byteVar*>("s"));
  ASSERT_EQ(state->getValue<byteVar*>("s2.test.s"), 0);

  //get the executables

  auto execs = state->executables();

  // s = 1
  ASSERT_EQ(execs.size(), 4);
  for(auto exec : execs)
  {
    ASSERT_EQ(exec->src, state);
  }

  std::list<transition*>::iterator exec = execs.begin();
  auto state11 = state->fire(*exec++);

  ASSERT_EQ(state11->get("s1")->getValue<byteVar*>("test.s"), 255);
  ASSERT_EQ(state11->get("s2")->getValue<byteVar*>("test.s"), 255);

  ASSERT_EQ(state11->get<process*>("s1.test")->getLocation(), 3);
  ASSERT_EQ(state11->get<process*>("s2.test")->getLocation(), 3);

  auto state21 = state->fire(*exec++);

  ASSERT_EQ(state21->get("s1")->get("test")->getValue<byteVar*>("s"), 1);
  ASSERT_EQ(state21->get("s2")->get("test")->getValue<byteVar*>("s"), 255);

  auto oneLoc = dynamic_cast<process*>(state21->get("s1.test"))->getLocation();

  ASSERT_EQ(dynamic_cast<process*>(state21->get("s1.test"))->getLocation(), 3);
  ASSERT_EQ(dynamic_cast<process*>(state21->get("s2.test"))->getLocation(), 3);

  auto state12 = state->fire(*exec++);

  ASSERT_EQ(state12->getValue<byteVar*>("s1.test.s"), 255);
  ASSERT_EQ(state12->getValue<byteVar*>("s2.test.s"), 1);

  ASSERT_EQ(state12->get<process*>("s1.test")->getLocation(), 3);
  ASSERT_EQ(state12->get<process*>("s2.test")->getLocation(), 3);

  auto state22 = state->fire(*exec++);

  ASSERT_EQ(state22->getValue<byteVar*>("s1.test.s"), 1);
  ASSERT_EQ(state22->getValue<byteVar*>("s2.test.s"), 1);

  ASSERT_EQ(dynamic_cast<process*>(state22->get("s1.test"))->getLocation(), 3);
  ASSERT_EQ(dynamic_cast<process*>(state22->get("s2.test"))->getLocation(), 3);
}