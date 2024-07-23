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
  auto original_loader = std::make_unique<promela_loader>("byte s;\n\
                           active proctype test(){\n\
                            do\n\
                            :: s == 0 -> s = 1;\n\
                            :: s == 1 -> s = 2;\n\
                            :: s == 2 -> s = 0;\n\
                            :: s == 2 -> s = 1;\n\
                            od;\n\
                          }");

  auto originalFSM = original_loader->getAutomata();
  // Create the initial state for both automata
  auto state = initState::createInitState(originalFSM.get());
  auto initState = state;

  auto copy = state->deepCopy();

  ASSERT_TRUE(state->hash() == copy->hash());

  // Check the state space
  state = state->fire({"test", {4, 4}});
  copy = state->deepCopy();

  ASSERT_TRUE(state->hash() == copy->hash());
}

TEST_F(SimilarityTest, simpleSimilarMutant) {
  auto original_loader = std::make_unique<promela_loader>("byte s;\n\
                           active proctype test(){\n\
                            do\n\
                            :: s == 0 -> s = 1;\n\
                            :: s == 1 -> s = 0;\n\
                            od;\n\
                          }");

  auto originalFSM = original_loader->getAutomata();
  // Create the initial state for both automata
  auto original_state = initState::createInitState(originalFSM.get());

  auto mutant_loader = std::make_unique<promela_loader>("byte s;\n\
                           active proctype test(){\n\
                            do\n\
                            :: s == 0 -> s = 1;\n\
                            :: s == 1 -> s = 1;\n\
                            od;\n\
                          }");

  auto mutantFSM = mutant_loader->getAutomata();
  // Create the initial state for both automata
  auto mutant_state = initState::createInitState(mutantFSM.get());

  ASSERT_TRUE(original_state->get<byteVar*>("s")->hash() == mutant_state->get<byteVar*>("s")->hash());
  ASSERT_TRUE(original_state->get<process*>("test")->hash() == mutant_state->get<process*>("test")->hash());

  ASSERT_TRUE(original_state->hash() == mutant_state->hash());

  original_state = original_state->fire({"test", {4, 4}});
  mutant_state = mutant_state->fire({"test", {4, 4}});

  ASSERT_TRUE(original_state->get<byteVar*>("s")->hash() == mutant_state->get<byteVar*>("s")->hash());
  ASSERT_TRUE(original_state->get<process*>("test")->hash() == mutant_state->get<process*>("test")->hash());

  auto original_variables = original_state->get<program*>("")->getVariablesVector();
  auto mutant_variables = mutant_state->get<program*>("")->getVariablesVector();
  for(int i = 0; i < original_variables.size(); i++) {
    if(original_variables[i]->getLocalName() != mutant_variables[i]->getLocalName()) {
      original_variables[i]->print();
      mutant_variables[i]->print();
    }
    if(original_variables[i]->hash() != mutant_variables[i]->hash()) {
      original_variables[i]->print();
      mutant_variables[i]->print();
    }
  }

  ASSERT_TRUE(original_state->hash() == mutant_state->hash());

  original_state = original_state->fire({"test", {5, 5}});
  mutant_state = mutant_state->fire({"test", {5, 5}});

  ASSERT_FALSE(original_state->hash() == mutant_state->hash());
}