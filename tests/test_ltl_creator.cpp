#include "../src/core/logic/ltl.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <iterator>
#include <memory>
#include <string>

// Define a fixture for the tests
class LTLTransformerTest : public ::testing::Test {
protected:
  void SetUp() override {}

  void TearDown() override
  {
    // Common teardown code that will be called after each test
  }
  std::string flows_model = "/test_files/appendClaimTest/flows.pml";
  std::string current_path = std::filesystem::current_path();

  bool compareFiles(const std::string & filePath1, const std::string & filePath2)
  {
    std::ifstream file1(filePath1), file2(filePath2);
    std::string line1, line2;

    if (!file1.is_open() || !file2.is_open()) {
      std::cerr << "Error opening one of the files." << std::endl;
      return false;
    }

    while (!file1.eof() || !file2.eof()) {
      std::getline(file1, line1);
      std::getline(file2, line2);

      if (file1.eof() != file2.eof() || line1 != line2) {
        return false; // Files are different
      }
    }
    return true; // Files are identical
  }
};

TEST_F(LTLTransformerTest, formulaStringToNeverClaim_Globally)
{
  auto formula = "[](x)";
  auto result = transformLTLStringToNeverClaim(formula);
  std::string expected_result =
      "never{/*!([](x))*/\nT0_init:\n\tif\n\t::(1)->gotoT0_init\n\t::(!x)->gotoaccept_all\n\tfi;\naccept_all:\n\tskip\n}\n";
  expected_result.erase(remove(expected_result.begin(), expected_result.end(), ' '), expected_result.end());
  result.erase(remove(result.begin(), result.end(), ' '), result.end());
  ASSERT_EQ(result, expected_result);
}

TEST_F(LTLTransformerTest, formulaStringToNeverClaim_Finally)
{
  auto formula = "<>(x)";
  auto result = transformLTLStringToNeverClaim(formula);
  std::string expected_result = "never{/*!(<>(x))*/\naccept_init:\n\tif\n\t::(!x)->gotoaccept_init\n\tfi;\n}\n";
  expected_result.erase(remove(expected_result.begin(), expected_result.end(), ' '), expected_result.end());
  result.erase(remove(result.begin(), result.end(), ' '), result.end());
  ASSERT_EQ(result, expected_result);
}

TEST_F(LTLTransformerTest, formulaStringToNeverClaim_Liveness)
{
  auto formula = "[]((!(x)) -> <>x)";
  auto result = transformLTLStringToNeverClaim(formula);
  std::string expected_result = "never{/*!([]((!(x))-><>x))*/"
                                "\nT0_init:\n\tif\n\t::(1)->gotoT0_init\n\t::(!x)->gotoaccept_S2\n\tfi;\naccept_S2:\n\tif\n\t::"
                                "(!x)->gotoaccept_S2\n\tfi;\n}\n";
  expected_result.erase(remove(expected_result.begin(), expected_result.end(), ' '), expected_result.end());
  result.erase(remove(result.begin(), result.end(), ' '), result.end());
  ASSERT_EQ(result, expected_result);
}

TEST_F(LTLTransformerTest, AppendNeverClaim_Globally)
{
  auto filePath = current_path + flows_model;
  // Copy the original file to a temporary file
  auto tempFilePath = current_path + "/test_files/appendClaimTest/flows_temp.pml";
  std::filesystem::copy_file(filePath, tempFilePath, std::filesystem::copy_options::overwrite_existing);
  auto formula = "[](x)";
  auto result = appendClaimToFile(tempFilePath, formula);
  ASSERT_EQ(result, 1);
  std::string expected_file = current_path + "/test_files/appendClaimTest/flows_always_expected.pml";
  // Compare the temporary file with the expected file
  auto compareResult = compareFiles(tempFilePath, expected_file);
  ASSERT_TRUE(compareResult);
  // Remove the temporary file
  std::filesystem::remove(tempFilePath);
}

TEST_F(LTLTransformerTest, AppendToNeverClaim_Finally)
{
  auto filePath = current_path + flows_model;
  auto tempFilePath = current_path + "/test_files/appendClaimTest/flows_temp.pml";
  std::filesystem::copy_file(filePath, tempFilePath, std::filesystem::copy_options::overwrite_existing);
  auto formula = "<>(x)";
  auto result = appendClaimToFile(tempFilePath, formula);
  ASSERT_EQ(result, 1);
  std::string expected_file = current_path + "/test_files/appendClaimTest/flows_eventually_expected.pml";
  // Compare the temporary file with the expected file
  auto compareResult = compareFiles(tempFilePath, expected_file);
  ASSERT_TRUE(compareResult);
  // Remove the temporary file
  std::filesystem::remove(filePath);
}

TEST_F(LTLTransformerTest, AppendToNeverClaim_Liveness)
{
  auto filePath = current_path + flows_model;
  auto tempFilePath = current_path + "/test_files/appendClaimTest/flows_temp.pml";
  std::filesystem::copy_file(filePath, tempFilePath, std::filesystem::copy_options::overwrite_existing);
  auto formula = "[]((!(x)) -> <>x)";
  auto result = appendClaimToFile(tempFilePath, formula);
  ASSERT_EQ(result, 1);
  std::string expected_file = current_path + "/test_files/appendClaimTest/flows_liveness_expected.pml";
  // Compare the temporary file with the expected file
  auto compareResult = compareFiles(tempFilePath, expected_file);
  ASSERT_TRUE(compareResult);
  // Remove the temporary file
  std::filesystem::remove(filePath);
}