#include <filesystem>
#include <gtest/gtest.h>
#include <memory>

#include "../../src/core/semantic/semantic.hpp"
#include "../../src/algorithm/explore.hpp"
#include "../../src/core/automata/fsmEdge.hpp"
#include "../../src/core/automata/fsmNode.hpp"
#include "../../src/promela/parser/promela_loader.hpp"

// Define a fixture for the tests
class SimilarityTest : public ::testing::Test {
protected:
  void SetUp() override {}

  void TearDown() override
  {
    // Common teardown code that will be called after each test
  }
  std::string helloWorld ="byte s;\n\
                           active proctype test(){\n\
                            do\n\
                            :: s == 0 -> s = 1;\n\
                            :: s == 1 -> s = 2;\n\
                            :: s == 2 -> s = 0;\n\
                            :: s == 2 -> s = 1;\n\
                            od;\n\
                          }";

  std::string helloChanRDV = "mtype {UP, DOWN};\n\
                          chan c = [0] of {mtype};\n\
                          active proctype foo(){\n\
                            do\n\
                            :: c!UP;\n\
                            :: c!DOWN;\n\
                            :: true -> skip;\n\
                            od;\n\
                          }\n\
                          active proctype bar(){\n\
                            mtype x;\n\
                            do\n\
                            :: c?x;\n\
                            od;\n\
                          }";

  std::string helloChanRDV_ = "chan c = [0] of {byte};\n\
                          active proctype foo(){\n\
                            do\n\
                            :: c!1;\n\
                            od;\n\
                          }\n\
                          active proctype bar(){\n\
                            do\n\
                            :: c?_;\n\
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
                                }";

        std::string minepump_toy_3 = "mtype = {stop, start, medium}\n\
                                chan cCmd = [0] of {mtype};\n\
                                chan cLevel = [0] of {mtype};\n\
                                active proctype controller(){\n\
                                do\n\
                                ::	cCmd?_;\n\
                                    cCmd!stop;\n\
                                ::	cLevel?_;\n\
                                od;\n\
                              }\n\
                              active proctype user(){\n\
                                do\n\
                                ::	cCmd!start;\n\
                                  cCmd?_;\n\
                                od;\n\
                              }\n\
                              active proctype watersensor(){\n\
                                do\n\
                                ::	atomic {\n\
                                    skip;\n\
                                    cLevel!medium;\n\
                                  };\n\
                                od;}";
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

TEST_F(SimilarityTest, simpleSimilar)
{
  const TVL * tvl = nullptr;

  auto original_loader = std::make_unique<promela_loader>(helloWorld, tvl);
  auto originalFSM = original_loader->getAutomata();
  // Create the initial state for both automata
  auto state = initState::createInitState(originalFSM.get(), tvl);
  auto initState = state;

  ASSERT_TRUE(initState->isSame(state));

  //get the executables
  auto execs = state->executables();
  // s == 0

  auto exec = execs.front();

  //go the the next state
  state = state->fire(exec);
  ASSERT_FALSE(initState->isSame(state));
  ASSERT_TRUE(initState->get("s")->isSame(state->get("s")));
  ASSERT_FALSE(initState->get<process*>("test")->getLocation() == state->get<process*>("test")->getLocation());

  execs = state->executables();
  // s = 1
  exec = execs.front();

  state = state->fire(exec);
  auto state1_at23 = state;
  
  ASSERT_FALSE(initState->isSame(state1_at23));
  ASSERT_FALSE(initState->get("s")->isSame(state1_at23->get("s")));
  ASSERT_TRUE(initState->get<process*>("test")->getLocation() == state1_at23->get<process*>("test")->getLocation());

  execs = state->executables();
  // s == 1
  exec = execs.front();
  
  state = state->fire(exec);
  ASSERT_FALSE(initState->isSame(state));
  ASSERT_FALSE(initState->get("s")->isSame(state->get("s")));
  ASSERT_FALSE(initState->get<process*>("test")->getLocation() == state->get<process*>("test")->getLocation());

  ASSERT_FALSE(state1_at23->isSame(state));
  ASSERT_TRUE(state1_at23->get("s")->isSame(state->get("s")));
  ASSERT_FALSE(state1_at23->get<process*>("test")->getLocation() == state->get<process*>("test")->getLocation());

  execs = state->executables();
  // s = 2
  exec = execs.front();

  state = state->fire(exec);
  
  auto state2_at24 = state;

  ASSERT_EQ(state->getValue<byteVar*>("test.s"), 2);
  std::unique_ptr<byteVar> s2_(new byteVar(2));
  ASSERT_TRUE(state2_at24->get("s")->isSame(s2_.get()));

  execs = state->executables();
  // s == 2
  // s == 2
  auto exec0 = execs.front();
  auto exec1 = execs.back();

  auto s1 = state->fire(exec0);
  ASSERT_FALSE(initState->get<process*>("test")->getLocation() == s1->get<process*>("test")->getLocation());

  auto s2 = state->fire(exec1);
  ASSERT_FALSE(initState->get<process*>("test")->getLocation() == s2->get<process*>("test")->getLocation());

  auto state0_at25 = s1->Post().front();
  auto state1_at26 = s2->Post().front();

  ASSERT_TRUE(initState->isSame(state0_at25));
  ASSERT_TRUE(initState->get("s")->isSame(state0_at25->get("s")));
  ASSERT_TRUE(initState->get<process*>("test")->getLocation() == state0_at25->get<process*>("test")->getLocation());

  ASSERT_FALSE(initState->isSame(state1_at26));
  ASSERT_FALSE(initState->get("s")->isSame(state1_at26->get("s")));
  ASSERT_TRUE(initState->get<process*>("test")->getLocation() == state1_at26->get<process*>("test")->getLocation());

  ASSERT_TRUE(state1_at23->isSame(state1_at26));
  ASSERT_TRUE(state1_at23->get("s")->isSame(state1_at26->get("s")));
  ASSERT_TRUE(state1_at23->get<process*>("test")->getLocation() == state1_at26->get<process*>("test")->getLocation());

  ASSERT_FALSE(state1_at23->isSame(state0_at25));
  ASSERT_FALSE(state1_at23->get("s")->isSame(state0_at25->get("s")));
  ASSERT_TRUE(state1_at23->get<process*>("test")->getLocation() == state0_at25->get<process*>("test")->getLocation());

  ASSERT_FALSE(state2_at24->isSame(state0_at25));
  ASSERT_FALSE(state2_at24->get("s")->isSame(state0_at25->get("s")));
  ASSERT_TRUE(state2_at24->get<process*>("test")->getLocation() == state0_at25->get<process*>("test")->getLocation());

  ASSERT_FALSE(state2_at24->isSame(state1_at26));
  ASSERT_FALSE(state2_at24->get("s")->isSame(state1_at26->get("s")));
  ASSERT_TRUE(state2_at24->get<process*>("test")->getLocation() == state1_at26->get<process*>("test")->getLocation());
}

TEST_F(SimilarityTest, simpleSimilarChanRDV)
{
  auto original_loader = std::make_unique<promela_loader>("mtype {UP, DOWN};\n\
                          chan c = [0] of {mtype};\n\
                          active proctype foo(){\n\
                            do\n\
                            :: c!UP;\n\
                            :: c!DOWN;\n\
                            :: true -> skip;\n\
                            od;\n\
                          }\n\
                          active proctype bar(){\n\
                            mtype x;\n\
                            do\n\
                            :: c?x;\n\
                            od;\n\
                          }");

  // Create the initial state for both automata
  auto state = initState::createInitState(original_loader->getAutomata().get());
  auto initState = state;

  ASSERT_TRUE(initState->isSame(state));

  state = initState->fire({{"foo", 5}});

  ASSERT_TRUE(initState->getValue<mtypeVar*>("bar.x") == 0);

  ASSERT_FALSE(initState->isSame(state));
  ASSERT_FALSE(initState->get("bar.x") == state->get("bar.x"));
  ASSERT_TRUE(initState->get<process*>("foo")->getLocation() == state->get<process*>("foo")->getLocation());
  ASSERT_TRUE(initState->get<process*>("bar")->getLocation() == state->get<process*>("bar")->getLocation());

  state = initState->fire({{"foo", 7}});

  ASSERT_TRUE(initState->getValue<mtypeVar*>("bar.x") == 0);
  ASSERT_TRUE(state->getValue<mtypeVar*>("bar.x") == 0);

  ASSERT_FALSE(initState->isSame(state));
  ASSERT_TRUE(initState->getValue<mtypeVar*>("bar.x") == state->getValue<mtypeVar*>("bar.x"));
  ASSERT_FALSE(initState->get<process*>("foo")->getLocation() == state->get<process*>("foo")->getLocation());
  ASSERT_TRUE(initState->get<process*>("bar")->getLocation() == state->get<process*>("bar")->getLocation());
}

TEST_F(SimilarityTest, flushChanRDV)
{
  auto original_loader = std::make_unique<promela_loader>("chan c = [0] of {byte};\n\
                          active proctype sender(){\n\
                          do\n\
                          :: c!1;\n\
                          od;\n\
                          }\n\
                          active proctype receiver(){\n\
                          do\n\
                          :: c?_;\n\
                          od;\n\
                          }");

  // Create the initial state for both automata
  auto state = initState::createInitState(original_loader->getAutomata().get());
  auto initState = state;

  auto flush = initState->fire({{"sender", 4}});

  ASSERT_TRUE(initState->get<process*>("sender")->isSame(flush->get<process*>("sender")));
  ASSERT_TRUE(initState->get<process*>("receiver")->isSame(flush->get<process*>("receiver")));

  //ASSERT_TRUE(initState->isSame(flush)); 
}

TEST_F(SimilarityTest, minepump)
{
  auto original_loader = std::make_unique<promela_loader>("../tests/models/minepump/original_.pml");
  auto originalFSM = original_loader->getAutomata();
  // Create the initial state for both automata
  auto state = initState::createInitState(originalFSM.get());
  auto initState = state;

  auto stateOn = initState->fire({"user", {93, 96}})->fire({"controller", {18}});
  auto stateOff = initState->fire({"user", {94, 96}})->fire({"controller", {18}});

  ASSERT_FALSE(stateOn->isSame(stateOff));
  ASSERT_FALSE(stateOn->get<mtypeVar*>("uwants")->isSame(stateOff->get<mtypeVar*>("uwants")));
  ASSERT_FALSE(stateOn->get<process*>("controller")->isSame(stateOff->get<process*>("controller")));
  ASSERT_FALSE(stateOn->get<mtypeVar*>("controller.pcommand")->isSame(stateOff->get<mtypeVar*>("controller.pcommand")));

  ASSERT_TRUE(stateOn->get<process*>("methanealarm")->isSame(stateOff->get<process*>("methanealarm")));
  ASSERT_TRUE(stateOn->get<process*>("methanesensor")->isSame(stateOff->get<process*>("methanesensor")));
  ASSERT_TRUE(stateOn->get<process*>("watersensor")->isSame(stateOff->get<process*>("watersensor")));
}
