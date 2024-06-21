#include "../../src/core/semantic/variable/state/state.hpp"
#include "../../src/formulas/formulaCreator.hpp"
#include "../../src/mutants/modelAnalyzer.hpp"

#include <filesystem>
#include <gtest/gtest.h>
#include <memory>

#include "../TestFilesUtils.hpp"

// Define a fixture for the tests
class ModelAnalyzerTest : public ::testing::Test {
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

TEST_F(ModelAnalyzerTest, model_analysis_array)
{
  auto original_file_path = testFilesUtils->array_model();
  ModelAnalyzer modelAnalyzer(original_file_path);
  modelAnalyzer.analyzeVariableUpdates();
}

TEST_F(ModelAnalyzerTest, model_analysis_two_trafficLights)
{
  auto original_file_path = testFilesUtils->two_trafficLight_model_original();
  ModelAnalyzer modelAnalyzer(original_file_path);
  modelAnalyzer.analyzeVariableUpdates();
}

TEST_F(ModelAnalyzerTest, model_analysis_trafficLight)
{
  auto original_file_path = testFilesUtils->trafficLight_model_original();
  ModelAnalyzer modelAnalyzer(original_file_path);
  modelAnalyzer.analyzeVariableUpdates();
}

// TEST_F(ModelAnalyzerTest, model_analysis_flows_model)
// {
//   auto original_file_path = testFilesUtils->flows_model_original();
//   ModelAnalyzer modelAnalyzer(original_file_path);
//   modelAnalyzer.analyzeVariableUpdates();
// }

// TEST_F(ModelAnalyzerTest, model_analysis_peterson_model)
// {
//   auto original_file_path = testFilesUtils->minepump_model_original();
//   ModelAnalyzer modelAnalyzer(original_file_path);
//   modelAnalyzer.analyzeVariableUpdates();
// }