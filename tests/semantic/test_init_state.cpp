#include <filesystem>
#include <gtest/gtest.h>
#include <memory>

#include "../../src/algorithm/explore.hpp"
#include "../../src/core/automata/fsmEdge.hpp"
#include "../../src/core/automata/fsmNode.hpp"

// Define a fixture for the tests
class stateInit : public ::testing::Test {
protected:
  void SetUp() override {}

  void TearDown() override
  {
    // Common teardown code that will be called after each test
  }
  std::string foo = "/test_files/basic/hello_world.pml";
  std::string current_path = std::filesystem::current_path();
};


TEST_F(stateInit, InitStateHelloWorld)
{
  const TVL * tvl = nullptr;
  auto file_path = current_path + foo;
  auto original_loader = std::make_unique<promela_loader>(file_path, tvl);
  auto originalFSM = original_loader->getAutomata();
  // Create the initial state for both automata
  auto state = initState::createInitState(originalFSM, tvl);
  ASSERT_TRUE(state != nullptr);
  
}
