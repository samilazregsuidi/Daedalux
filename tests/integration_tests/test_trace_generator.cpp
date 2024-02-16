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
  std::string test1 = "/test_files/mutants/array.pml";
  std::string test1_mutant = "/test_files/mutants/array_mutant.pml";
  std::string minepump = "/models/minepump/minepump.pml";
  std::string minepump_mutant = "/models/minepump/mutants/mutant_1.pml";
  std::string current_path = std::filesystem::current_path();
};

TEST_F(TraceGenerator, SimpleTraceHelloWorld_SameFSM)
{
  const TVL * tvl = nullptr;
  auto file_path = current_path + test1;
  auto original_loader = std::make_unique<promela_loader>(file_path, tvl);
  auto originalFSM = original_loader->getAutomata();

  auto trace_size = 15;
  auto trace_1 = generateNegativeTraces(originalFSM, originalFSM, trace_size);
  ASSERT_EQ(trace_1->size(), trace_size);
}

TEST_F(TraceGenerator, SimpleTraceHelloWorld_DifferentFSM)
{
  const TVL * tvl = nullptr;
  auto file_path = current_path + test1;
  auto original_loader = new promela_loader(file_path, tvl);
  auto originalFSM = original_loader->getAutomata();
  delete original_loader;
  auto file_path_mutant = current_path + test1_mutant;
  auto mutant_loader = std::make_unique<promela_loader>(file_path_mutant, tvl);
  auto mutantFSM = mutant_loader->getAutomata();
  auto trace_size = 12;
  auto trace = generateNegativeTraces(originalFSM, mutantFSM, trace_size);
  ASSERT_EQ(trace->size(), trace_size);
}

TEST_F(TraceGenerator, TraceReport_DifferentFSM)
{
  const TVL * tvl = nullptr;
  auto file_path = current_path + test1;
  auto original_loader = new promela_loader(file_path, tvl);
  auto originalFSM = original_loader->getAutomata();
  delete original_loader;
  auto file_path_mutant = current_path + test1_mutant;
  auto mutant_loader = std::make_unique<promela_loader>(file_path_mutant, tvl);
  auto mutantFSM = mutant_loader->getAutomata();
  auto trace_size = 12;
  auto traceReport = generateTraces(originalFSM, mutantFSM, 1, trace_size, tvl);
  ASSERT_EQ(traceReport->getGoodTraces().size(), 1);
  ASSERT_EQ(traceReport->getBadTraces().size(), 1);
  auto good_trace = *traceReport->getGoodTraces().begin();
  auto bad_trace = *traceReport->getBadTraces().begin();
  ASSERT_EQ(good_trace->size(), trace_size);
  ASSERT_EQ(bad_trace->size(), trace_size);
}

// TEST_F(TraceGenerator, interactiveDebuggingHelloWorld)
// {
//   const TVL * tvl = nullptr;
//   auto file_path = current_path + test1;
//   auto original_loader = new promela_loader(file_path, tvl);
//   auto originalFSM = original_loader->getAutomata();
//   delete original_loader;
//   auto trace_size = 10;
//   auto trace = interactiveDebugging(originalFSM, 10, tvl);
//   ASSERT_EQ(trace->size(), trace_size);
// }

// TEST_F(TraceGenerator, interactiveDebuggingMinepump)
// {
//   const TVL * tvl = nullptr;
//   auto file_path = current_path + minepump;
//   auto original_loader = new promela_loader(file_path, tvl);
//   auto originalFSM = original_loader->getAutomata();
//   delete original_loader;
//   auto trace_size = 10;
//   auto trace = interactiveDebugging(originalFSM, 10, tvl);
//   ASSERT_EQ(trace->size(), trace_size);
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
