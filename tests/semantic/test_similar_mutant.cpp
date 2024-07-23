#include <filesystem>
#include <gtest/gtest.h>
#include <memory>

#include "../../src/core/semantic/semantic.hpp"
#include "../../src/algorithm/explore.hpp"
#include "../../src/core/automata/fsmEdge.hpp"
#include "../../src/core/automata/fsmNode.hpp"
#include "../../src/promela/parser/promela_loader.hpp"

// Define a fixture for the tests
class SimilarityMutantTest : public ::testing::Test {
protected:
  void SetUp() override {}

  void TearDown() override
  {
    // Common teardown code that will be called after each test
  }
};

TEST_F(SimilarityMutantTest, SimilarityMutantTest1)
{
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

  ASSERT_TRUE(original_state->isSame(mutant_state));

  original_state = original_state->fire({"test", {4, 4}});
  mutant_state = mutant_state->fire({"test", {4, 4}});

  ASSERT_TRUE(original_state->isSame(mutant_state));

  original_state = original_state->fire({"test", {5, 5}});
  mutant_state = mutant_state->fire({"test", {5, 5}});

  ASSERT_FALSE(original_state->isSame(mutant_state));
}

TEST_F(SimilarityMutantTest, SimilarityMutantTest2)
{
  auto original_loader = std::make_unique<promela_loader>("../tests/test_files/mutants/flows/flows.pml");
  auto mutant_loader = std::make_unique<promela_loader>("../tests/test_files/mutants/flows/flows_mutant.pml");

  auto orginalState = initState::createInitState(original_loader->getAutomata().get());
  auto mutantState = initState::createInitState(mutant_loader->getAutomata().get());

  ASSERT_TRUE(orginalState->isSame(mutantState));

  orginalState = orginalState->fire({"I", {12, 13}});
  mutantState = mutantState->fire({"I", {12, 13}});

  ASSERT_TRUE(orginalState->isSame(mutantState));

  orginalState = orginalState->fire({{"I", 16}});
  mutantState = mutantState->fire({{"I", 16}});

  ASSERT_FALSE(orginalState->isSame(mutantState));
}

