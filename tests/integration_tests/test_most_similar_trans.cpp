#include <filesystem>
#include <gtest/gtest.h>
#include <memory>

#include "../../src/algorithm/explore.hpp"
#include "../../src/core/automata/fsmEdge.hpp"
#include "../../src/core/automata/fsmNode.hpp"

// Define a fixture for the tests
class SimilarityTest : public ::testing::Test {
protected:
  void SetUp() override {}

  void TearDown() override
  {
    // Common teardown code that will be called after each test
  }
  std::string file1 = "/test_files/basic/array.pml";
  std::string file2 = "/test_files/basic/flows.pml";
  std::string current_path = std::filesystem::current_path();
};

TEST_F(SimilarityTest, DistinctState_EmptyList)
{
  printf("Comparing distinct states of an empty list\n");
  const TVL * tvl = nullptr;
  auto file_path = current_path + file2;
  auto loader = std::make_unique<promela_loader>(file_path, tvl);
  auto originalFSM = loader->getAutomata().get();
  // Create the initial state for both automatas
  auto current_state_original = initState::createInitState(originalFSM, tvl);
  auto post_states_original = std::list<state *>{current_state_original};
  std::list<state *> post_states_mutant;
  ASSERT_EQ(post_states_original.empty(), false);
  ASSERT_EQ(post_states_mutant.empty(), true);
  auto different_states = distinct_states(post_states_original, post_states_mutant);
  ASSERT_EQ(different_states, post_states_original);
}

TEST_F(SimilarityTest, DistinctStates_DifferentFSM)
{
  auto file_path1 = current_path + file1;
  auto file_path2 = current_path + file2;
  const TVL * tvl = nullptr;
  auto loader1 = std::make_unique<promela_loader>(file_path1, tvl);
  auto loader2 = std::make_unique<promela_loader>(file_path2, tvl);
  auto myFSM = loader1->getAutomata().get();
  auto mutant = loader2->getAutomata().get();
  // Create the initial state for both automatas
  auto current_state_original = initState::createInitState(myFSM, tvl);
  auto current_state_mutant = initState::createInitState(mutant, tvl);
  printf("Created initial states\n");
  printf("Computing post states\n");
  auto post_states_original = current_state_original->Post();
  auto post_states_mutant = current_state_mutant->Post();
  ASSERT_EQ(post_states_original.empty(), false);
  ASSERT_EQ(post_states_mutant.empty(), false);
  auto different_states = distinct_states(post_states_original, post_states_mutant);
  ASSERT_EQ(different_states.empty(), false);
  ASSERT_EQ(different_states.size(), post_states_original.size());
}

TEST_F(SimilarityTest, DistinctStates_SameFSM)
{
  printf("Comparing distinct states of the same FSM\n");
  const TVL * tvl = nullptr;
  // Create the initial state for both automatas
  auto file_path1 = current_path + file1;
  auto loader = std::make_unique<promela_loader>(file_path1, tvl);
  auto myFSM = loader->getAutomata().get();
  auto current_state_original = initState::createInitState(myFSM, tvl);
  printf("Created initial states\n");
  auto post_states_original = current_state_original->Post();
  auto post_states_mutant = current_state_original->Post();
  ASSERT_EQ(post_states_original.empty(), false);
  ASSERT_EQ(post_states_mutant.empty(), false);
  ASSERT_EQ(post_states_original.size(), post_states_mutant.size());
  auto different_states = distinct_states(post_states_original, post_states_mutant);
  ASSERT_EQ(different_states.empty(), true);
}

TEST_F(SimilarityTest, SameStateDelta_ShouldBe0)
{
  const TVL * tvl = nullptr;
  auto file_path1 = current_path + file1;
  auto loader = std::make_unique<promela_loader>(file_path1, tvl);
  auto myFSM = loader->getAutomata().get();
  auto current_state = initState::createInitState(myFSM, tvl);
  auto delta = current_state->delta(current_state);
  auto expected = 0.0;
  printf("Computing delta for the same state\n");
  current_state->print();
  printf("Delta: %f\n", delta);
  ASSERT_EQ(delta - expected < 0.0001, true);
}

TEST_F(SimilarityTest, DifferentStateDelta_ShouldBe0)
{
  const TVL * tvl = nullptr;
  auto file_path1 = current_path + file1;
  auto loader = std::make_unique<promela_loader>(file_path1, tvl);
  auto myFSM = loader->getAutomata().get();
  auto current_state = initState::createInitState(myFSM, tvl);
  auto post_state = current_state->Post().front();
  printf("Computing delta for the states\n");
  current_state->print();
  post_state->print();
  auto delta = current_state->delta(post_state);
  auto expected = 0.85;
  printf("Delta: %f\n", delta);
  ASSERT_EQ(delta - expected < 0.0001, true);
  auto post_post_state = post_state->Post().front();
  printf("Computing delta for the states\n");
  current_state->print();
  post_post_state->print();
  delta = current_state->delta(post_post_state);
  expected = 0.85;
  printf("Delta: %f\n", delta);
  ASSERT_EQ(delta - expected < 0.0001, true);
}