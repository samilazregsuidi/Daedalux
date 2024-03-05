#include "../../src/core/semantic/variable/state/state.hpp"
#include "../../src/formulas/formulaCreator.hpp"
#include "../../src/mutants/mutantAnalyzer.hpp"

#include <filesystem>
#include <gtest/gtest.h>
#include <memory>

// Define a fixture for the tests
class MutantHandlerTest : public ::testing::Test {
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

TEST_F(MutantHandlerTest, mutant_generation_array)
{
  auto original_file_path = testFilesUtils->array_model_never();
  MutantAnalyzer mutantAnalyzer(original_file_path);
  auto number_of_mutants = 5;
  mutantAnalyzer.createMutants(number_of_mutants);
  auto mutants = mutantAnalyzer.getMutantFilePaths();
  ASSERT_EQ(mutants.size(), number_of_mutants);

  // Assert that all mutants are created
  for (auto mutant : mutants) {
    std::ifstream file(mutant);
    ASSERT_TRUE(file.good());
    // Ensure that the program is different from the original
    std::ifstream original_file(original_file_path);
    std::string original_program((std::istreambuf_iterator<char>(original_file)), std::istreambuf_iterator<char>());
    std::ifstream mutant_file(mutant);
    std::string mutant_program((std::istreambuf_iterator<char>(mutant_file)), std::istreambuf_iterator<char>());
    ASSERT_NE(original_program, mutant_program);
    // Remove the file
    std::filesystem::remove(mutant);
  }
}

TEST_F(MutantHandlerTest, mutant_generation_flows)
{
  auto original_file_path = testFilesUtils->flows_model();
  MutantAnalyzer mutantAnalyzer(original_file_path);
  auto number_of_mutants = 5;
  mutantAnalyzer.createMutants(number_of_mutants);
  auto mutants = mutantAnalyzer.getMutantFilePaths();
  ASSERT_EQ(mutants.size(), number_of_mutants);
  // Assert that all mutants are created
  for (auto mutant : mutants) {
    std::ifstream file(mutant);
    ASSERT_TRUE(file.good());
    // Ensure that the program is different from the original
    std::ifstream original_file(original_file_path);
    std::string original_program((std::istreambuf_iterator<char>(original_file)), std::istreambuf_iterator<char>());
    std::ifstream mutant_file(mutant);
    std::string mutant_program((std::istreambuf_iterator<char>(mutant_file)), std::istreambuf_iterator<char>());
    ASSERT_NE(original_program, mutant_program);
    // Remove the file
    std::filesystem::remove(mutant);
  }
}

TEST_F(MutantHandlerTest, AnalyzeMutants)
{
  auto original_file_path = testFilesUtils->array_model_never();
  auto mutant_file_path = testFilesUtils->array_mutant_never();
  MutantAnalyzer mutantAnalyzer(original_file_path, {mutant_file_path});
  auto trace_length = 15;
  mutantAnalyzer.analyzeMutants(trace_length);
  auto mutants = mutantAnalyzer.getMutantFilePaths();
  for (auto mutant : mutants) {
    std::ifstream file(mutant);
    ASSERT_TRUE(file.good());
    // Remove the file
    std::filesystem::remove(mutant);
  }
}

TEST_F(MutantHandlerTest, EnhanceSpecificationToKillMutants_1Mutant)
{
  auto original_file_path = testFilesUtils->array_model_original();
  LTLClaimsProcessor::removeClaimFromFile(original_file_path);
  auto mutant_file_path = testFilesUtils->array_model_mutant();
  LTLClaimsProcessor::removeClaimFromFile(mutant_file_path);
  std::vector<std::string> mutants = {mutant_file_path};
  MutantAnalyzer mutantAnalyzer(original_file_path, mutants);
  auto number_of_mutants = 5;
  auto trace_length = 15;
  mutantAnalyzer.enhanceSpecification(number_of_mutants, trace_length);
  // Check that the new never claim can kill the mutants
  auto [killed_mutants, alive_mutants] = mutantAnalyzer.killMutants();
  ASSERT_EQ(killed_mutants.size(), 1);
  ASSERT_TRUE(alive_mutants.empty());
}

TEST_F(MutantHandlerTest, EnhanceSpecificationToKillMutants_1MutantAlt)
{
  auto original_file_path = testFilesUtils->array_model_original();
  LTLClaimsProcessor::removeClaimFromFile(original_file_path);
  auto mutant_file_path = testFilesUtils->array_model_mutant_alt();
  LTLClaimsProcessor::removeClaimFromFile(mutant_file_path);
  std::vector<std::string> mutants = {mutant_file_path};
  MutantAnalyzer mutantAnalyzer(original_file_path, mutants);
  auto number_of_mutants = 5;
  auto trace_length = 15;
  mutantAnalyzer.enhanceSpecification(number_of_mutants, trace_length);
  // Check that the new never claim can kill the mutants
  auto [killed_mutants, alive_mutants] = mutantAnalyzer.killMutants();
  ASSERT_EQ(killed_mutants.size(), 1);
  ASSERT_TRUE(alive_mutants.empty());
}

TEST_F(MutantHandlerTest, EnhanceSpecificationToKillMutants_2Mutants)
{
  auto original_file_path = testFilesUtils->array_model_original();
  LTLClaimsProcessor::removeClaimFromFile(original_file_path);
  auto mutant_file_path_1 = testFilesUtils->array_model_mutant();
  LTLClaimsProcessor::removeClaimFromFile(mutant_file_path_1);
  auto mutant_file_path_2 = testFilesUtils->array_model_mutant_alt();
  LTLClaimsProcessor::removeClaimFromFile(mutant_file_path_2);
  std::vector<std::string> mutants = {mutant_file_path_1, mutant_file_path_2};
  MutantAnalyzer mutantAnalyzer(original_file_path, mutants);
  auto number_of_mutants = 5;
  auto trace_length = 15;
  mutantAnalyzer.enhanceSpecification(number_of_mutants, trace_length);
  // Check that the new never claim can kill the mutants
  auto [killed_mutants, alive_mutants] = mutantAnalyzer.killMutants();
  ASSERT_EQ(killed_mutants.size(), 2);
  ASSERT_TRUE(alive_mutants.empty());
}

TEST_F(MutantHandlerTest, EnhanceSpecificationToKillMutantsTrafficLight)
{
  auto original_file_path = testFilesUtils->trafficLight_model_original();
  LTLClaimsProcessor::removeClaimFromFile(original_file_path);
  auto mutant_file_path_1 = testFilesUtils->trafficLight_model_mutant();
  LTLClaimsProcessor::removeClaimFromFile(mutant_file_path_1);
  std::vector<std::string> mutants = {mutant_file_path_1};
  MutantAnalyzer mutantAnalyzer(original_file_path, mutants);
  auto number_of_mutants = 5;
  auto trace_length = 15;
  mutantAnalyzer.enhanceSpecification(number_of_mutants, trace_length);
  // Check that the new never claim can kill the mutants
  auto [killed_mutants, alive_mutants] = mutantAnalyzer.killMutants();
  ASSERT_EQ(killed_mutants.size(), 1);
  ASSERT_TRUE(alive_mutants.empty());
}

TEST_F(MutantHandlerTest, EnhanceSpecificationToKillMutantsTrafficLight_TwoModels)
{
  auto original_file_path = testFilesUtils->trafficLight_model_original();
  LTLClaimsProcessor::removeClaimFromFile(original_file_path);
  auto mutant_file_path_1 = testFilesUtils->trafficLight_model_mutant();
  LTLClaimsProcessor::removeClaimFromFile(mutant_file_path_1);
  auto mutant_file_path_2 = testFilesUtils->trafficLight_model_mutant_alt();
  LTLClaimsProcessor::removeClaimFromFile(mutant_file_path_2);
  std::vector<std::string> mutants = {mutant_file_path_1, mutant_file_path_2};
  MutantAnalyzer mutantAnalyzer(original_file_path, mutants);
  auto number_of_mutants = 5;
  auto trace_length = 15;
  mutantAnalyzer.enhanceSpecification(number_of_mutants, trace_length);
  // Check that the new never claim can kill the mutants
  auto [killed_mutants, alive_mutants] = mutantAnalyzer.killMutants();
  ASSERT_EQ(killed_mutants.size(), 2);
  ASSERT_TRUE(alive_mutants.empty());
}

TEST_F(MutantHandlerTest, EnhanceSpecification_3Processes)
{
  auto original_file_path = testFilesUtils->process_model_original();
  LTLClaimsProcessor::removeClaimFromFile(original_file_path);
  auto mutant_file_path_1 = testFilesUtils->process_model_mutant();
  LTLClaimsProcessor::removeClaimFromFile(mutant_file_path_1);
  std::vector<std::string> mutants = {mutant_file_path_1};
  MutantAnalyzer mutantAnalyzer(original_file_path, mutants);
  auto number_of_mutants = 5;
  auto trace_length = 15;
  mutantAnalyzer.enhanceSpecification(number_of_mutants, trace_length);
  // Check that the new never claim can kill the mutants
  auto [killed_mutants, alive_mutants] = mutantAnalyzer.killMutants();
  ASSERT_EQ(killed_mutants.size(), 1);
  ASSERT_TRUE(alive_mutants.empty());
}

TEST_F(MutantHandlerTest, EnhanceSpecificationToKillMutantsFlows)
{
  auto original_file_path = testFilesUtils->flows_model_original();
  LTLClaimsProcessor::removeClaimFromFile(original_file_path);
  auto mutant_file_path_1 = testFilesUtils->flows_model_mutant();
  LTLClaimsProcessor::removeClaimFromFile(mutant_file_path_1);
  std::vector<std::string> mutants = {mutant_file_path_1};
  MutantAnalyzer mutantAnalyzer(original_file_path, mutants);
  auto number_of_mutants = 5;
  auto trace_length = 15;
  mutantAnalyzer.enhanceSpecification(number_of_mutants, trace_length);
  // Check that the new never claim can kill the mutants
  auto [killed_mutants, alive_mutants] = mutantAnalyzer.killMutants();
  ASSERT_EQ(killed_mutants.size(), 1);
  ASSERT_TRUE(alive_mutants.empty());
}