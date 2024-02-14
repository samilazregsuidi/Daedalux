#include <filesystem>
#include <gtest/gtest.h>
#include <memory>

#include "../../src/algorithm/explore.hpp"
#include "../../src/core/automata/fsmEdge.hpp"
#include "../../src/core/automata/fsmNode.hpp"

// Define a fixture for the tests
class TraceGenerator : public ::testing::Test {
protected:
  void SetUp() override {}

  void TearDown() override
  {
    // Common teardown code that will be called after each test
  }
  std::string foo = "/test_files/basic/hello_world.pml";
  std::string minepump = "/models/minepump/minepump.pml";
  std::string minepump_mutant = "/models/minepump/mutants/mutant_1.pml";
  std::string current_path = std::filesystem::current_path();
};

// TEST_F(TraceGenerator, SimpleTrace)
// {
//   const TVL * tvl = nullptr;
//   auto file_path = current_path + foo;
//   auto original_loader = std::make_unique<promela_loader>(file_path, tvl);
//   auto originalFSM = original_loader->getAutomata();
//   // Create the initial state for both automata
//   generateNegativeTraces(originalFSM, originalFSM, 5);
// }

TEST_F(TraceGenerator, SimpleTraceMinepump)
{
  const TVL * tvl = nullptr;
  auto file_path = current_path + minepump;
  auto original_loader = new promela_loader(file_path, tvl);
  auto originalFSM = original_loader->getAutomata();
  delete original_loader;

  // Load the mutant
  auto file_path_mutant = current_path + minepump_mutant;
  auto mutant_loader = std::make_unique<promela_loader>(file_path_mutant, tvl);
  auto mutantFSM = mutant_loader->getAutomata();
  // Create the initial state for both automata
  generateNegativeTraces(originalFSM, mutantFSM, 10);
}
