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
  std::string minepump = "/models/minepump/minepump.pml";
  std::string current_path = std::filesystem::current_path();
};

TEST_F(SimilarityTest, DistinctState_EmptyList)
{
  printf("Comparing distinct states of an empty list\n");
  const TVL * tvl = nullptr;
  auto file_path = current_path + minepump;
  auto loader = std::make_unique<promela_loader>(file_path, tvl);
  auto originalFSM = loader->getAutomata().get();
  // Create the initial state for both automata
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
  if(std::filesystem::current_path().string().find("macOS") == std::string::npos) {
    GTEST_SKIP();
  }
  auto file_path1 = current_path + file1;
  auto file_path2 = current_path + file2;
  const TVL * tvl = nullptr;
  auto loader1 = std::make_unique<promela_loader>(file_path1, tvl);
  auto loader2 = std::make_unique<promela_loader>(file_path2, tvl);
  auto myFSM = loader1->getAutomata().get();
  auto mutant = loader2->getAutomata().get();
  // Create the initial state for both automata
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
  // Create the initial state for both automata
  auto file_path = current_path + minepump;
  auto loader = std::make_unique<promela_loader>(file_path, tvl);
  auto myFSM = loader->getAutomata().get();
  printf("Created automata\n");
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
  auto file_path1 = current_path + minepump;
  auto loader = std::make_unique<promela_loader>(file_path1, tvl);
  auto myFSM = loader->getAutomata().get();
  auto current_state = initState::createInitState(myFSM, tvl);
  auto delta = current_state->delta(current_state);
  auto expected = 0.0;
  ASSERT_EQ(delta - expected < 0.0001, true);
}

TEST_F(SimilarityTest, DifferentStateDelta_ShouldNotBe0)
{
  const TVL * tvl = nullptr;
  auto file_path1 = current_path + minepump;
  auto loader = std::make_unique<promela_loader>(file_path1, tvl);
  auto myFSM = loader->getAutomata().get();
  auto current_state = initState::createInitState(myFSM, tvl);
  auto post_state = current_state->Post().front();
  auto delta = current_state->delta(post_state);
  auto expected = 0.008333;
  ASSERT_EQ(delta - expected < 0.00001, true);
  auto post_post_state = post_state->Post().front();
  delta = current_state->delta(post_post_state);
  expected = 0.011538;
  ASSERT_EQ(delta - expected < 0.00001, true);
}

TEST_F(SimilarityTest, MostSimilarStateEmptyList)
{
  const TVL * tvl = nullptr;
  auto file_path = current_path + minepump;
  auto loader = std::make_unique<promela_loader>(file_path, tvl);
  auto myFSM = loader->getAutomata().get();
  auto current_state = initState::createInitState(myFSM, tvl);
  std::list<state *> post_states;
  auto most_similar = most_similar_state(current_state, post_states);
  ASSERT_TRUE(most_similar == nullptr);
}

TEST_F(SimilarityTest, FlowMostSimilarStateOneElement)
{
  const TVL * tvl = nullptr;
  auto file_path = current_path + minepump;
  auto loader = std::make_unique<promela_loader>(file_path, tvl);
  auto myFSM = loader->getAutomata().get();
  auto current_state = initState::createInitState(myFSM, tvl);
  auto post_state = current_state->Post().front();
  std::list<state *> post_states = {post_state};
  auto most_similar = most_similar_state(current_state, post_states);
  ASSERT_TRUE(most_similar != nullptr);
  post_state->print();
  most_similar->print();
  ASSERT_TRUE(most_similar == post_state);
}

// TEST_F(SimilarityTest, FlowMostSimilarStateOfSameState)
// {
//   const TVL * tvl = nullptr;
//   auto file_path = current_path + file2;
//   auto loader = std::make_unique<promela_loader>(file_path, tvl);
//   auto myFSM = loader->getAutomata().get();
//   auto current_state = initState::createInitState(myFSM, tvl);
//   auto post_states = current_state->Post();
//   post_states.push_back(current_state);
//   ASSERT_TRUE(post_states.size() > 0);
//   auto most_similar = most_similar_state(current_state, post_states);
//   ASSERT_TRUE(most_similar != nullptr);
//   ASSERT_EQ(most_similar, current_state);
// }

// TEST_F(SimilarityTest, MostSimilarStateOneElement)
// {
//   const TVL * tvl = nullptr;
//   auto file_path = current_path + minepump;
//   auto loader = std::make_unique<promela_loader>(file_path, tvl);
//   auto myFSM = loader->getAutomata().get();
//   auto current_state = initState::createInitState(myFSM, tvl);
//   auto post_state = current_state->Post().front();
//   std::list<state *> post_states = {post_state};
//   auto most_similar = most_similar_state(current_state, post_states);
//   ASSERT_TRUE(most_similar == post_state);
// }

// TEST_F(SimilarityTest, MostSimilarStateOfSameState)
// {
//   const TVL * tvl = nullptr;
//   auto file_path = current_path + minepump;
//   auto loader = std::make_unique<promela_loader>(file_path, tvl);
//   auto myFSM = loader->getAutomata().get();
//   auto current_state = initState::createInitState(myFSM, tvl);
//   auto post_states = current_state->Post();
//   post_states.push_back(current_state);
//   ASSERT_TRUE(post_states.size() > 0);
//   auto most_similar = most_similar_state(current_state, post_states);
//   ASSERT_TRUE(most_similar != nullptr);
//   ASSERT_EQ(most_similar, current_state);
// }