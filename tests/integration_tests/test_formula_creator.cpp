#include "../../src/core/semantic/variable/state/state.hpp"
#include "../../src/core/semantic/variable/state/initState.hpp"
#include "../../src/formulas/formulaCreator.hpp"
#include "../../src/promela/parser/promela_loader.hpp"

#include <filesystem>
#include <gtest/gtest.h>
#include <memory>

// Define a fixture for the tests
class FormulaCreatorTest : public ::testing::Test {
protected:
  void SetUp() override {}

  void TearDown() override
  {
    // Common teardown code that will be called after each test
  }
  std::string array_model = "/test_files/mutants/array.pml";
  std::string array_model_mutant = "/test_files/mutants/array_mutant.pml";
  std::string minepump = "/models/minepump/minepump.pml";
  std::string flows_model = "/test_files/basic/flows.pml";
  std::string current_path = std::filesystem::current_path();
};

TEST_F(FormulaCreatorTest, test_buildVariableValueMap_one_state)
{
  const TVL * tvl = nullptr;
  auto file_path = current_path + array_model;
  auto original_loader = std::make_unique<promela_loader>(file_path, tvl);
  auto fsm1 = original_loader->getAutomata();
  auto current_state_fsm1 = initState::createInitState(fsm1.get(), tvl);
  std::shared_ptr<state> current_state_fsm1_ptr(current_state_fsm1);
  const std::vector<std::shared_ptr<state>> states = std::vector<std::shared_ptr<state>>{current_state_fsm1_ptr};

  auto value_map = formulaCreator::buildVariableValueMap(states);
  for (auto var : value_map) {
    auto values = var.second;
    ASSERT_EQ(values.size(), 1);
  }
}

TEST_F(FormulaCreatorTest, test_buildVariableValueMap_two_states)
{
  const TVL * tvl = nullptr;
  auto file_path = current_path + array_model;
  auto original_loader = std::make_unique<promela_loader>(file_path, tvl);
  auto fsm1 = original_loader->getAutomata();
  auto current_state_fsm1 = initState::createInitState(fsm1.get(), tvl);
  std::shared_ptr<state> current_state_fsm1_ptr(current_state_fsm1);
  auto next_state = current_state_fsm1->Post().front()->Post().front()->Post().front()->Post().front()->Post().front();
  std::shared_ptr<state> next_state_ptr(next_state);
  const std::vector<std::shared_ptr<state>> states =
      std::vector<std::shared_ptr<state>>{current_state_fsm1_ptr, next_state_ptr};

  ASSERT_EQ(states.size(), 2);
  ASSERT_FALSE(current_state_fsm1_ptr->isSame(next_state_ptr.get()));

  auto value_map = formulaCreator::buildVariableValueMap(states);
  // Given that we have two states, the value map should at least for one variable have two values.
  bool has_two_values = false;
  for (auto var : value_map) {
    auto values = var.second;
    if (values.size() == 2) {
      has_two_values = true;
    }
    ASSERT_TRUE(values.size() <= 2);
  }
  ASSERT_TRUE(has_two_values);
}

TEST_F(FormulaCreatorTest, test_buildVariableValueMap_two_states_flows)
{
  const TVL * tvl = nullptr;
  auto file_path = current_path + flows_model;
  auto original_loader = std::make_unique<promela_loader>(file_path, tvl);
  auto fsm1 = original_loader->getAutomata();
  auto current_state_fsm1 = initState::createInitState(fsm1.get(), tvl);
  std::shared_ptr<state> current_state_fsm1_ptr(current_state_fsm1);
  auto next_state = current_state_fsm1->Post().front()->Post().front()->Post().front();
  std::shared_ptr<state> next_state_ptr(next_state);
  const std::vector<std::shared_ptr<state>> states =
      std::vector<std::shared_ptr<state>>{current_state_fsm1_ptr, next_state_ptr};

  ASSERT_EQ(states.size(), 2);
  ASSERT_FALSE(current_state_fsm1_ptr->isSame(next_state_ptr.get()));

  auto value_map = formulaCreator::buildVariableValueMap(states);
  // Given that we have two states, the value map should at least for one variable have two values.
  bool has_two_values = false;
  for (auto var : value_map) {
    auto values = var.second;
    if (values.size() == 2) {
      std::cout << "Variable: " << var.first << " has two different values" << std::endl;
      has_two_values = true;
    }
    ASSERT_TRUE(values.size() <= 2);
  }
  ASSERT_TRUE(has_two_values);
}

TEST_F(FormulaCreatorTest, groupStates_singleState)
{
  const TVL * tvl = nullptr;
  auto file_path = current_path + flows_model;
  auto original_loader = std::make_unique<promela_loader>(file_path, tvl);
  auto fsm1 = original_loader->getAutomata();
  auto current_state_fsm1 = initState::createInitState(fsm1.get(), tvl);
  std::shared_ptr<state> current_state_fsm1_ptr(current_state_fsm1);
  const std::vector<std::shared_ptr<state>> states = std::vector<std::shared_ptr<state>>{current_state_fsm1_ptr};

  formulaCreator::groupStates(states);
}

TEST_F(FormulaCreatorTest, groupStates_array)
{
  const TVL * tvl = nullptr;
  auto file_path = current_path + array_model;
  auto original_loader = std::make_unique<promela_loader>(file_path, tvl);
  auto fsm1 = original_loader->getAutomata();
  auto current_state_fsm1 = initState::createInitState(fsm1.get(), tvl);
  std::shared_ptr<state> current_state_fsm1_ptr(current_state_fsm1);
  auto next_state = current_state_fsm1->Post().front()->Post().front()->Post().front()->Post().front()->Post().front();
  ASSERT_FALSE(current_state_fsm1_ptr->isSame(next_state));
  auto next_next_state = next_state->Post().front()->Post().front()->Post().front()->Post().front()->Post().front();
  ASSERT_FALSE(next_state->isSame(next_next_state));
  auto next_next_next_state = next_next_state->Post().front()->Post().front()->Post().front()->Post().front()->Post().front();
  ASSERT_FALSE(next_next_state->isSame(next_next_next_state));
  std::shared_ptr<state> next_state_ptr(next_state);
  std::shared_ptr<state> next_next_state_ptr(next_next_state);
  std::shared_ptr<state> next_next_next_state_ptr(next_next_next_state);
  const std::vector<std::shared_ptr<state>> states = std::vector<std::shared_ptr<state>>{
      current_state_fsm1_ptr, next_state_ptr, next_next_state_ptr, next_next_next_state_ptr};
  std::vector<std::shared_ptr<state>>{current_state_fsm1_ptr, next_state_ptr};

  ASSERT_EQ(states.size(), 4);
  formulaCreator::groupStates(states);
}

TEST_F(FormulaCreatorTest, groupStates_flows)
{
  const TVL * tvl = nullptr;
  auto file_path = current_path + flows_model;
  auto original_loader = std::make_unique<promela_loader>(file_path, tvl);
  auto fsm1 = original_loader->getAutomata();
  auto current_state_fsm1 = initState::createInitState(fsm1.get(), tvl);
  std::shared_ptr<state> current_state_fsm1_ptr(current_state_fsm1);
  auto next_state = current_state_fsm1->Post().front()->Post().front()->Post().front();
  std::shared_ptr<state> next_state_ptr(next_state);
  const std::vector<std::shared_ptr<state>> states =
      std::vector<std::shared_ptr<state>>{current_state_fsm1_ptr, next_state_ptr};

  ASSERT_EQ(states.size(), 2);
  ASSERT_FALSE(current_state_fsm1_ptr->isSame(next_state_ptr.get()));
  formulaCreator::groupStates(states);
}

TEST_F(FormulaCreatorTest, distinguishStates_array_same_states)
{
  const TVL * tvl = nullptr;
  auto file_path = current_path + array_model;
  auto original_loader = std::make_unique<promela_loader>(file_path, tvl);
  auto fsm1 = original_loader->getAutomata();
  auto current_state_fsm1 = initState::createInitState(fsm1.get(), tvl);
  std::shared_ptr<state> current_state_fsm1_ptr(current_state_fsm1);
  auto next_state = current_state_fsm1->Post().front()->Post().front()->Post().front()->Post().front()->Post().front();
  std::shared_ptr<state> next_state_ptr(next_state);
  const std::vector<std::shared_ptr<state>> include_states =
      std::vector<std::shared_ptr<state>>{current_state_fsm1_ptr, next_state_ptr};

  ASSERT_EQ(include_states.size(), 2);
  formulaCreator::distinguishStates(include_states, include_states);
}

TEST_F(FormulaCreatorTest, distinguishStates_array)
{
  const TVL * tvl = nullptr;
  auto file_path = current_path + array_model;
  auto original_loader = std::make_unique<promela_loader>(file_path, tvl);
  auto fsm1 = original_loader->getAutomata();
  auto current_state_fsm1 = initState::createInitState(fsm1.get(), tvl);
  std::shared_ptr<state> current_state_fsm1_ptr(current_state_fsm1);
  auto next_state = current_state_fsm1->Post().front()->Post().front()->Post().front()->Post().front()->Post().front();
  auto next_next_state = next_state->Post().front()->Post().front()->Post().front()->Post().front()->Post().front();
  auto next_next_next_state = next_next_state->Post().front()->Post().front()->Post().front()->Post().front()->Post().front();
  std::shared_ptr<state> next_state_ptr(next_state);
  std::shared_ptr<state> next_next_state_ptr(next_next_state);
  std::shared_ptr<state> next_next_next_state_ptr(next_next_next_state);
  const std::vector<std::shared_ptr<state>> include_states =
      std::vector<std::shared_ptr<state>>{current_state_fsm1_ptr, next_state_ptr};
  const std::vector<std::shared_ptr<state>> exclude_states =
      std::vector<std::shared_ptr<state>>{next_next_state_ptr, next_next_next_state_ptr};

  ASSERT_EQ(include_states.size(), 2);
  ASSERT_EQ(exclude_states.size(), 2);
  formulaCreator::distinguishStates(include_states, exclude_states);
}

TEST_F(FormulaCreatorTest, distinguishStates_flows)
{
  const TVL * tvl = nullptr;
  auto file_path = current_path + flows_model;
  auto original_loader = std::make_unique<promela_loader>(file_path, tvl);
  auto fsm1 = original_loader->getAutomata();
  auto current_state_fsm1 = initState::createInitState(fsm1.get(), tvl);
  std::shared_ptr<state> current_state_fsm1_ptr(current_state_fsm1);
  auto successors = current_state_fsm1->Post();
  auto next_state = successors.front();
  std::shared_ptr<state> next_state_ptr(next_state);
  const std::vector<std::shared_ptr<state>> include_states = std::vector<std::shared_ptr<state>>{
      current_state_fsm1_ptr,
  };

  const std::vector<std::shared_ptr<state>> exclude_states = std::vector<std::shared_ptr<state>>{next_state_ptr};
  ASSERT_EQ(include_states.size(), 1);
  ASSERT_EQ(exclude_states.size(), 1);
  formulaCreator::distinguishStates(include_states, exclude_states);
}

TEST_F(FormulaCreatorTest, formulaStringToNeverClaim_Globally)
{
  auto formula = "[](x)";
  auto result = formulaCreator::formulaStringToNeverClaim(formula);
  std::string expected_result =
      "never{/*!([](x))*/\nT0_init:\n\tif\n\t::(1)->gotoT0_init\n\t::(!x)->gotoaccept_all\n\tfi;\naccept_all:\n\tskip\n}\n";
  expected_result.erase(remove(expected_result.begin(), expected_result.end(), ' '), expected_result.end());
  result.erase(remove(result.begin(), result.end(), ' '), result.end());
  ASSERT_EQ(result, expected_result);
}

TEST_F(FormulaCreatorTest, formulaStringToNeverClaim_Finally)
{
  auto formula = "<>(x)";
  auto result = formulaCreator::formulaStringToNeverClaim(formula);
  std::cout << "Result: " << result << std::endl;
  std::string expected_result = "never{/*!(<>(x))*/\naccept_init:\n\tif\n\t::(!x)->gotoaccept_init\n\tfi;\n}\n";
  expected_result.erase(remove(expected_result.begin(), expected_result.end(), ' '), expected_result.end());
  result.erase(remove(result.begin(), result.end(), ' '), result.end());
  ASSERT_EQ(result, expected_result);
}

TEST_F(FormulaCreatorTest, formulaStringToNeverClaim_Liveness)
{
  auto formula = "[]((!(x)) -> <>x)";
  auto result = formulaCreator::formulaStringToNeverClaim(formula);
  std::string expected_result = "never{/*!([]((!(x))-><>x))*/"
                                "\nT0_init:\n\tif\n\t::(1)->gotoT0_init\n\t::(!x)->gotoaccept_S2\n\tfi;\naccept_S2:\n\tif\n\t::"
                                "(!x)->gotoaccept_S2\n\tfi;\n}\n";
  expected_result.erase(remove(expected_result.begin(), expected_result.end(), ' '), expected_result.end());
  result.erase(remove(result.begin(), result.end(), ' '), result.end());
  ASSERT_EQ(result, expected_result);
}
