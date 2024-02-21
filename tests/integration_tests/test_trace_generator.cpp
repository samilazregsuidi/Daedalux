#include <filesystem>
#include <gtest/gtest.h>
#include <memory>

#include "../../src/algorithm/traceGenerator.hpp"
#include "../../src/core/automata/fsmEdge.hpp"
#include "../../src/core/automata/fsmNode.hpp"

// Define a fixture for the tests
class TraceGeneratorTest : public ::testing::Test {
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

TEST_F(TraceGeneratorTest, SimpleTraceHelloWorld_SameFSM)
{
  const TVL * tvl = nullptr;
  auto file_path = current_path + test1;
  auto original_loader = std::make_unique<promela_loader>(file_path, tvl);
  auto originalFSM = original_loader->getAutomata();

  auto trace_size = 15;
  auto traceGen = std::make_unique<TraceGenerator>(originalFSM, originalFSM);
  auto trace = traceGen->generateNegativeTrace(trace_size);
  ASSERT_EQ(trace->size(), trace_size);
}

TEST_F(TraceGeneratorTest, SimpleTraceHelloWorld_SameFSM_IgnoreCommonPrefix)
{
  const TVL * tvl = nullptr;
  auto file_path = current_path + test1;
  auto original_loader = std::make_unique<promela_loader>(file_path, tvl);
  auto originalFSM = original_loader->getAutomata();
  auto trace_size = 15;
  bool ignore_common_prefix = true;
  auto traceGen = std::make_unique<TraceGenerator>(originalFSM, originalFSM);
  auto trace = traceGen->generateNegativeTrace(trace_size, ignore_common_prefix);
  // We should not be able to create a trace as the common prefix is ignored
  ASSERT_EQ(trace->size(), 0);
}

TEST_F(TraceGeneratorTest, SimpleTraceHelloWorld_DifferentFSM)
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
  auto traceGen = std::make_unique<TraceGenerator>(originalFSM, originalFSM);
  auto trace = traceGen->generateNegativeTrace(trace_size);
  ASSERT_EQ(trace->size(), trace_size);
}

TEST_F(TraceGeneratorTest, SimpleTraceHelloWorld_DifferentFSM_IgnoreCommonPrefix)
{
  const TVL * tvl = nullptr;
  auto file_path = current_path + test1;
  auto original_loader = new promela_loader(file_path, tvl);
  auto originalFSM = original_loader->getAutomata();
  delete original_loader;
  auto file_path_mutant = current_path + test1_mutant;
  auto mutant_loader = std::make_unique<promela_loader>(file_path_mutant, tvl);
  auto mutantFSM = mutant_loader->getAutomata();
  auto trace_size = 15;
  bool ignore_common_prefix = true;
  auto traceGen = std::make_unique<TraceGenerator>(originalFSM, mutantFSM);
  auto trace = traceGen->generatePositiveTrace(trace_size, ignore_common_prefix);
  // The trace will be shorter than the requested size as the common prefix is ignored
  auto expected_trace_size = 4;
  ASSERT_EQ(trace->size(), expected_trace_size);
}

TEST_F(TraceGeneratorTest, TraceReport_DifferentFSM)
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
  auto traceGen = std::make_unique<TraceGenerator>(originalFSM, mutantFSM);
  auto traceReport = traceGen->generateTraceReport(1, trace_size);
  ASSERT_EQ(traceReport->getGoodTraces().size(), 1);
  ASSERT_EQ(traceReport->getBadTraces().size(), 1);
  auto good_trace = *traceReport->getGoodTraces().begin();
  auto bad_trace = *traceReport->getBadTraces().begin();
  ASSERT_EQ(good_trace->size(), trace_size);
  ASSERT_EQ(bad_trace->size(), trace_size);
}

TEST_F(TraceGeneratorTest, TraceReport_RemoveCommonPrefixes)
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
  auto traceGen = std::make_unique<TraceGenerator>(originalFSM, mutantFSM);
  auto traceReport = traceGen->generateTraceReport(1, trace_size);
  ASSERT_EQ(traceReport->getGoodTraces().size(), 1);
  ASSERT_EQ(traceReport->getBadTraces().size(), 1);
  auto reduced_traceReport = traceReport->removeCommonPrefixes();
  ASSERT_EQ(reduced_traceReport->getGoodTraces().size(), 1);
  ASSERT_EQ(reduced_traceReport->getBadTraces().size(), 1);
  auto good_trace = *reduced_traceReport->getGoodTraces().begin();
  auto bad_trace = *reduced_traceReport->getBadTraces().begin();
  ASSERT_TRUE(good_trace->size() < trace_size);
  ASSERT_TRUE(bad_trace->size() < trace_size);
}

TEST_F(TraceGeneratorTest, DiscriminateBetweenTrace)
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
  auto traceGen = std::make_unique<TraceGenerator>(originalFSM, mutantFSM);
  auto traceReport = traceGen->generateTraceReport(1, trace_size);
  auto good_trace = *traceReport->getGoodTraces().begin();
  auto bad_trace = *traceReport->getBadTraces().begin();
  good_trace->findDistinguishingFormula(bad_trace);
}

// TEST_F(TraceGeneratorTest, interactiveDebuggingHelloWorld)
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

// TEST_F(TraceGeneratorTest, interactiveDebuggingMinepump)
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

// TEST_F(TraceGeneratorTest, SimpleTraceMinepump)
// {
//   const TVL * tvl = nullptr;
//   auto file_path = current_path + minepump;
//   auto original_loader = new promela_loader(file_path, tvl);
//   auto originalFSM = original_loader->getAutomata();
//   delete original_loader;

//   // Load the mutant
//   auto file_path_mutant = current_path + minepump_mutant;
//   auto mutant_loader = std::make_unique<promela_loader>(file_path_mutant, tvl);
//   auto mutantFSM = mutant_loader->getAutomata();
//   // Create the initial state for both automata
//   generateNegativeTraces(originalFSM, mutantFSM, 10);
// }
