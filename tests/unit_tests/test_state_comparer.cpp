#include "../../src/core/semantic/variable/state/state.hpp"
#include "../../src/formulas/formulaCreator.hpp"
#include "../../src/algorithm/utils/stateComparer.hpp"
#include "../../src/promela/parser/promela_loader.hpp"
#include "../../src/core/semantic/variable/state/initState.hpp"
#include "../TestFilesUtils.hpp"

#include <filesystem>
#include <gtest/gtest.h>
#include <memory>

// Define a fixture for the tests
class StateComparerTest : public ::testing::Test {
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

TEST_F(StateComparerTest, sameStatesArrayModel)
{
  auto original = initState::createInitState(std::make_unique<promela_loader>(testFilesUtils->array_model_original())->getAutomata().get());
  auto mutant = initState::createInitState(std::make_unique<promela_loader>(testFilesUtils->array_model_mutant())->getAutomata().get());

  std::list<std::shared_ptr<state>> originalStates; originalStates.push_back(std::shared_ptr<state>(original));
  std::list<std::shared_ptr<state>> mutantStates; mutantStates.push_back(std::shared_ptr<state>(mutant));

  original->print();
  mutant->print();

  ASSERT_TRUE(StateComparer::sameStates(originalStates, mutantStates, true));
}