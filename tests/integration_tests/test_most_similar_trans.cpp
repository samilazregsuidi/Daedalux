#include <filesystem>
#include <gtest/gtest.h>
#include <memory>

#include "../../src/algorithm/explore.hpp"
#include "../../src/core/automata/fsmEdge.hpp"
#include "../../src/core/automata/fsmNode.hpp"

// Define a fixture for the tests
class SimilarityTest : public ::testing::Test {
protected:
  void SetUp() override
  {
    const TVL * tvl = nullptr;
    auto file_path = current_path + file1;
    printf("Loading file: %s\n", file_path.c_str());
    auto loader = std::make_unique<promela_loader>(file_path, tvl);
    myFSM = loader->getAutomata().get();
    printf("Loaded file: %s\n", file_path.c_str());
  }

  void TearDown() override
  {
    // Common teardown code that will be called after each test
    delete myFSM;
  }

  fsm * myFSM;
  std::string file1 = "/test_files/basic/array.pml";
  std::string file2 = "/test_files/basic/flows.pml";
  std::string current_path = std::filesystem::current_path();
};

TEST_F(SimilarityTest, DistinctStates_SameFSM)
{
  printf("Comparing distinct states of the same FSM\n");
  const TVL * tvl = nullptr;
  // Create the initial state for both automatas
  auto current_state_original = initState::createInitState(myFSM, tvl);
  auto current_state_mutant = initState::createInitState(myFSM, tvl);
  printf("Created initial states\n");
  printf("Computing post states\n");
  auto post_states_original = current_state_original->Post();
  auto post_states_mutant = current_state_mutant->Post();
  ASSERT_EQ(post_states_original.empty(), false);
  ASSERT_EQ(post_states_mutant.empty(), false);
  ASSERT_EQ(post_states_original.size(), post_states_mutant.size());
  auto different_states = distinct_states(post_states_original, post_states_mutant);
  ASSERT_EQ(different_states.empty(), true);
}

// TEST_F(SimilarityTest, DistinctStates_DifferentFSM)
// {
//   auto file_path = current_path + file2;
//   printf("Loading file: %s\n", file_path.c_str());
//   const TVL * tvl = nullptr;
//   auto loader = std::make_unique<promela_loader>(file_path, tvl);
//   auto mutant = loader->getAutomata().get();
//   printf("Loaded file: %s\n", file_path.c_str());
//   // Create the initial state for both automatas
//   auto current_state_original = initState::createInitState(myFSM, tvl);
//   auto current_state_mutant = initState::createInitState(mutant, tvl);
//   printf("Created initial states\n");
//   printf("Computing post states\n");

//   auto post_states_original = current_state_original->Post();
//   auto post_states_mutant = current_state_mutant->Post();
//   ASSERT_EQ(post_states_original.empty(), false);
//   ASSERT_EQ(post_states_mutant.empty(), false);
//   auto different_states = distinct_states(post_states_original, post_states_mutant);
//   ASSERT_EQ(different_states.empty(), false);
//   delete mutant;
// }

TEST_F(SimilarityTest, MostSimilarTransition_DifferentFSM)
{
  auto file_path = current_path + file2;
  printf("Loading file: %s\n", file_path.c_str());
  const TVL * tvl = nullptr;
  auto loader = std::make_unique<promela_loader>(file_path, tvl);
  auto mutant = loader->getAutomata().get();
  printf("Loaded file: %s\n", file_path.c_str());
  // Create the initial state for both automatas
  auto current_state_original = initState::createInitState(myFSM, tvl);
  auto current_state_mutant = initState::createInitState(mutant, tvl);
  printf("Created initial states\n");
  auto transitions = current_state_original->executables();
  auto transitions_mutant = current_state_mutant->executables();
  printf("Computing most similar transition\n");
  ASSERT_EQ(transitions.empty(), false);
  ASSERT_EQ(transitions_mutant.empty(), false);
  delete mutant;
}