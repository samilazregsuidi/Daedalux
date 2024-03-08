#include <filesystem>
#include <gtest/gtest.h>
#include <memory>

#include "../../src/algorithm/traceGenerator.hpp"
#include "../../src/algorithm/fsmExplorer.hpp"
#include "../../src/formulas/formulaCreator.hpp"
#include "../TestFilesUtils.hpp"

// Define a fixture for the tests
class TraceGeneratorTest : public ::testing::Test {
protected:
  void SetUp() override
  {
    // Common setup code that will be called before each test
    std::string current_path = std::filesystem::current_path();
    testFilesUtils = std::make_unique<TestFilesUtils>(current_path);
  }

  void TearDown() override
  {
    // Common teardown code that will be called after each test
  }
  std::unique_ptr<TestFilesUtils> testFilesUtils;
};

TEST_F(TraceGeneratorTest, SimpleTraceHelloWorld_SameFSM)
{
  const TVL * tvl = nullptr;
  auto original_loader = std::make_unique<promela_loader>(testFilesUtils->array_model(), tvl);
  auto originalFSM = original_loader->getAutomata();
  auto trace_size = 15;
  auto traceGen = std::make_unique<TraceGenerator>(originalFSM, originalFSM);
  auto trace = traceGen->generateNegativeTrace(trace_size);
  ASSERT_EQ(trace->size(), trace_size);
}

TEST_F(TraceGeneratorTest, SimpleTraceHelloWorld_SameFSM_IgnoreCommonPrefix)
{
  const TVL * tvl = nullptr;
  auto original_loader = std::make_unique<promela_loader>(testFilesUtils->array_model(), tvl);
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
  auto original_loader = new promela_loader(testFilesUtils->array_model(), tvl);
  auto originalFSM = original_loader->getAutomata();
  delete original_loader;
  auto file_path_mutant = testFilesUtils->array_model_mutant();
  LTLClaimsProcessor::removeClaimFromFile(file_path_mutant);
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
  auto file_path = testFilesUtils->array_model();
  LTLClaimsProcessor::removeClaimFromFile(file_path);
  auto original_loader = new promela_loader(file_path, tvl);
  auto originalFSM = original_loader->getAutomata();
  delete original_loader;
  auto file_path_mutant = testFilesUtils->array_model_mutant();
  LTLClaimsProcessor::removeClaimFromFile(file_path_mutant);
  auto mutant_loader = std::make_unique<promela_loader>(file_path_mutant, tvl);
  auto mutantFSM = mutant_loader->getAutomata();
  auto trace_size = 15;
  bool ignore_common_prefix = true;
  auto traceGen = std::make_unique<TraceGenerator>(originalFSM, mutantFSM);
  auto trace = traceGen->generatePositiveTrace(trace_size, ignore_common_prefix);
  // The trace will be shorter than the requested size as the common prefix is ignored
  auto expected_trace_size = 7;
  ASSERT_EQ(trace->getStates().size(), expected_trace_size);
}

TEST_F(TraceGeneratorTest, TraceReport_DifferentFSM)
{
  const TVL * tvl = nullptr;
  auto file_path = testFilesUtils->array_model();
  LTLClaimsProcessor::removeClaimFromFile(file_path);
  auto original_loader = new promela_loader(file_path, tvl);
  auto originalFSM = original_loader->getAutomata();
  delete original_loader;
  auto file_path_mutant = testFilesUtils->array_model_mutant();
  LTLClaimsProcessor::removeClaimFromFile(file_path_mutant);
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

TEST_F(TraceGeneratorTest, DiscardMutant)
{
  const TVL * tvl = nullptr;
  auto file_path = testFilesUtils->array_model();
  LTLClaimsProcessor::removeClaimFromFile(file_path);
  auto original_loader = new promela_loader(file_path, tvl);
  auto originalFSM = original_loader->getAutomata();
  delete original_loader;
  auto file_path_mutant = testFilesUtils->array_model_mutant();
  LTLClaimsProcessor::removeClaimFromFile(file_path_mutant);
  auto mutant_loader = std::make_unique<promela_loader>(file_path_mutant, tvl);
  auto mutantFSM = mutant_loader->getAutomata();
  auto formula = fsmExplorer::discardMutant(originalFSM, mutantFSM);
  auto arr_0 = std::make_shared<VariableFormula>("array[0]");
  auto arr_1 = std::make_shared<VariableFormula>("array[1]");
  auto arr_2 = std::make_shared<VariableFormula>("array[2]");
  auto arr_3 = std::make_shared<VariableFormula>("array[3]");
  auto number_0 = std::make_shared<NumberConstant>(0);
  auto number_1 = std::make_shared<NumberConstant>(1);
  auto number_2 = std::make_shared<NumberConstant>(2);
  auto number_3 = std::make_shared<NumberConstant>(3);
  auto arr_0_eq_0 = std::make_shared<EqualsFormula>(arr_0, number_0);
  auto arr_1_eq_1 = std::make_shared<EqualsFormula>(arr_1, number_1);
  auto arr_2_eq_2 = std::make_shared<EqualsFormula>(arr_2, number_2);
  auto arr_3_eq_0 = std::make_shared<EqualsFormula>(arr_3, number_0);
  auto arr_3_leq_3 = std::make_shared<LargerEqualsFormula>(arr_3, number_3);
  auto eventually = std::make_shared<FinallyFormula>(arr_3_leq_3);
  auto next = std::make_shared<NextFormula>(eventually);
  auto previousState =
      formulaUtility::combineFormulas({arr_0_eq_0, arr_1_eq_1, arr_2_eq_2, arr_3_eq_0}, CombinationOperatorType::AND_Symbol);
  auto implies_Formula = std::make_shared<ImpliesFormula>(previousState, next);
  auto expected_formula = std::make_shared<GloballyFormula>(implies_Formula);
  std::cout << "Result: " << formula->toFormula() << std::endl;
  std::cout << "Expected: " << expected_formula->toFormula() << std::endl;
  ASSERT_TRUE(formula->isEquivalent(expected_formula));
}

// TEST_F(TraceGeneratorTest, DiscardMutant_Flows)
// {
//   const TVL * tvl = nullptr;
//   auto file_path = testFilesUtils->flows_model();
//   LTLClaimsProcessor::removeClaimFromFile(file_path);
//   auto original_loader = new promela_loader(file_path, tvl);
//   auto originalFSM = original_loader->getAutomata();
//   delete original_loader;
//   auto file_path_mutant = testFilesUtils->flows_model_mutant();
//   LTLClaimsProcessor::removeClaimFromFile(file_path_mutant);
//   auto mutant_loader = std::make_unique<promela_loader>(file_path_mutant, tvl);
//   auto mutantFSM = mutant_loader->getAutomata();
//   auto formula = fsmExplorer::discardMutant(originalFSM, mutantFSM);
//   std::cout << "Result: " << formula->toFormula() << std::endl;
// }

TEST_F(TraceGeneratorTest, DiscardMutant_TrafficLight)
{
  const TVL * tvl = nullptr;
  auto file_path = testFilesUtils->trafficLight_model_original();
  LTLClaimsProcessor::removeClaimFromFile(file_path);
  auto original_loader = new promela_loader(file_path, tvl);
  auto originalFSM = original_loader->getAutomata();
  delete original_loader;
  auto file_path_mutant = testFilesUtils->trafficLight_model_mutant();
  LTLClaimsProcessor::removeClaimFromFile(file_path_mutant);
  auto mutant_loader = std::make_unique<promela_loader>(file_path_mutant, tvl);
  auto mutantFSM = mutant_loader->getAutomata();
  auto formula = fsmExplorer::discardMutant(originalFSM, mutantFSM);
  auto state_Var = std::make_shared<VariableFormula>("state");
  auto yellow = std::make_shared<VariableFormula>("yellow");
  auto red = std::make_shared<VariableFormula>("red");
  auto stateVar_eq_yellow = std::make_shared<EqualsFormula>(state_Var, yellow);
  auto stateVar_eq_red = std::make_shared<EqualsFormula>(state_Var, red);
  auto eventually = std::make_shared<FinallyFormula>(stateVar_eq_red);
  auto next = std::make_shared<NextFormula>(eventually);
  auto implies_Formula = std::make_shared<ImpliesFormula>(stateVar_eq_yellow, next);
  auto expected_formula = std::make_shared<GloballyFormula>(implies_Formula);
  ASSERT_TRUE(formula->isEquivalent(expected_formula));
}

TEST_F(TraceGeneratorTest, RemoveCommonPrefixes)
{
  const TVL * tvl = nullptr;
  auto file_path = testFilesUtils->array_model();
  LTLClaimsProcessor::removeClaimFromFile(file_path);
  auto original_loader = new promela_loader(file_path, tvl);
  auto originalFSM = original_loader->getAutomata();
  delete original_loader;
  auto file_path_mutant = testFilesUtils->array_model_mutant();
  LTLClaimsProcessor::removeClaimFromFile(file_path_mutant);
  auto mutant_loader = std::make_unique<promela_loader>(file_path_mutant, tvl);
  auto mutantFSM = mutant_loader->getAutomata();
  auto trace_size = 12;
  auto traceGen = std::make_unique<TraceGenerator>(originalFSM, mutantFSM);
  auto report = traceGen->generateTraceReport(1, trace_size);
  auto good_trace = *report->getGoodTraces().begin();
  auto bad_trace = *report->getBadTraces().begin();
  auto reduced_traces = formulaCreator::removeCommonPrefixes(good_trace, bad_trace);
  auto states_removed_good = good_trace->size() - reduced_traces.first->size();
  auto states_removed_bad = bad_trace->size() - reduced_traces.second->size();
  auto transition_removed_good = good_trace->getTransitions().size() - reduced_traces.first->getTransitions().size();
  auto transition_removed_bad = bad_trace->getTransitions().size() - reduced_traces.second->getTransitions().size();
  // The reduced traces should be shorter than the original traces
  ASSERT_TRUE(reduced_traces.first->size() < trace_size);
  ASSERT_TRUE(reduced_traces.second->size() < trace_size);
  // The number of states removed from the traces should be the same
  ASSERT_TRUE(states_removed_good == states_removed_bad);
  // The number of transitions removed from the traces should be the same
  ASSERT_TRUE(transition_removed_good == transition_removed_bad);
  // The two traces should have the same first state
  auto first_state_good = reduced_traces.first->getStates().front();
  auto first_state_bad = reduced_traces.second->getStates().front();
  ASSERT_TRUE(first_state_good->isSame(first_state_bad.get()));
  // The next state should be different
  auto second_state_good = reduced_traces.first->getStates().at(1);
  auto second_state_bad = reduced_traces.second->getStates().at(1);
  ASSERT_FALSE(second_state_good->isSame(second_state_bad.get()));
}

TEST_F(TraceGeneratorTest, RemoveCommonPrefixes_TheTwoMethodShouldReturnTheSameResult)
{
  const TVL * tvl = nullptr;
  auto file_path = testFilesUtils->array_model();
  LTLClaimsProcessor::removeClaimFromFile(file_path);
  auto original_loader = new promela_loader(file_path, tvl);
  auto originalFSM = original_loader->getAutomata();
  delete original_loader;
  auto file_path_mutant = testFilesUtils->array_model_mutant();
  LTLClaimsProcessor::removeClaimFromFile(file_path_mutant);
  auto mutant_loader = std::make_unique<promela_loader>(file_path_mutant, tvl);
  auto mutantFSM = mutant_loader->getAutomata();
  auto trace_size = 12;
  auto traceGen = std::make_unique<TraceGenerator>(originalFSM, mutantFSM);
  auto report = traceGen->generateTraceReport(1, trace_size);
  auto good_trace = *report->getGoodTraces().begin();
  auto bad_trace = *report->getBadTraces().begin();
  auto reduced_traces = formulaCreator::removeCommonPrefixes(good_trace, bad_trace);
  report->removeCommonPrefixes(); // This method should return the same result as the previous method
  good_trace = *report->getGoodTraces().begin();
  bad_trace = *report->getBadTraces().begin();
  ASSERT_TRUE(reduced_traces.first->size() == good_trace->size());
  ASSERT_TRUE(reduced_traces.second->size() == bad_trace->size());
}