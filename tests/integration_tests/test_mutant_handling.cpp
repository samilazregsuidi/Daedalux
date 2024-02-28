#include "../../src/core/semantic/variable/state/state.hpp"
#include "../../src/formulas/formulaCreator.hpp"
#include "../../src/mutants/mutantAnalyzer.hpp"

#include <filesystem>
#include <gtest/gtest.h>
#include <memory>

// Define a fixture for the tests
class MutantHandlerTest : public ::testing::Test {
protected:
  void SetUp() override {}

  void TearDown() override
  {
    // Common teardown code that will be called after each test
  }

  std::string array_model = "/test_files/mutants/array.pml";
  std::string array_model_mutant = "/test_files/mutants/array_mutant.pml";
  std::string array_model_never = "/test_files/mutants/array_never.pml";
  std::string array_model_mutant_never = "/test_files/mutants/array_mutant_never.pml";
  std::string flows_model = "/test_files/basic/flows.pml";
  std::string current_path = std::filesystem::current_path();
};

TEST_F(MutantHandlerTest, mutant_generation_array)
{
  auto original_file_path = current_path + array_model_never;
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
  auto original_file_path = current_path + flows_model;
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

TEST_F(MutantHandlerTest, analyzeMutants)
{
  auto original_file_path = current_path + array_model_never;
  auto mutant_file_path = current_path + array_model_mutant_never;
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

TEST_F(MutantHandlerTest, test_mutant_killing)
{
  // TODO: No done yet
  auto original_file_path = current_path + array_model_never;
  auto mutant_file_path = current_path + array_model_mutant_never;
  std::vector<std::string> mutants = {mutant_file_path};
  MutantAnalyzer mutantAnalyzer = MutantAnalyzer(original_file_path, mutants);
  // auto [killed_mutants, alive_mutants] = mutantAnalyzer.killMutants();
  // ASSERT_EQ(killed_mutants.size(), 0);
  // ASSERT_EQ(alive_mutants.size(), 1);
}

TEST_F(MutantHandlerTest, enhanceSpecification_neverClaimExists)
{
  auto original_file_path = current_path + array_model_never;
  auto mutant_file_path = current_path + array_model_mutant_never;
  std::vector<std::string> mutants = {mutant_file_path};
  MutantAnalyzer mutantAnalyzer(original_file_path, mutants);
  auto number_of_mutants = 5;
  auto trace_length = 15;
  mutantAnalyzer.enhanceSpecification(number_of_mutants, trace_length);
}

TEST_F(MutantHandlerTest, enhanceSpecification_NoNeverClaimExists)
{
  auto original_file_path = current_path + array_model;
  auto mutant_file_path = current_path + array_model_mutant;
  std::vector<std::string> mutants = {mutant_file_path};
  MutantAnalyzer mutantAnalyzer(original_file_path, mutants);
  auto number_of_mutants = 5;
  auto trace_length = 15;
  mutantAnalyzer.enhanceSpecification(number_of_mutants, trace_length);
}

