#include "../../src/algorithm/explore.hpp"
#include <gtest/gtest.h>

namespace fs = std::filesystem;

class DISABLED_ICSE_TEST : public ::testing::Test {
protected:
  void SetUp() override {
    // Common setup code that will be called before each test
    mc = std::make_unique<ltlModelChecker>();
  }
  void TearDown() override
  {
    // Common teardown code that will be called after each test
  }
  std::unique_ptr<ltlModelChecker> mc;
};

// Test case for the explore function
TEST_F(DISABLED_ICSE_TEST, END_2_END_MINEPUMP)
{
  const TVL * tvl = nullptr;
  std::string current_path = fs::current_path();
  auto icse_file = "/models/minepump/minepump.pml";
  auto file_path = current_path + icse_file;
  printf("Loading file: %s\n", file_path.c_str());
  auto loader = std::make_unique<promela_loader>(file_path, tvl);
  auto automata = loader->getAutomata();
  mc->startNestedDFS(automata.get(), tvl);
}

// // Test case for the explore function
// TEST_F(ICSE_TEST, END_2_END_ELEVATOR)
// {
//   const TVL * tvl = nullptr;
//   std::string current_path = fs::current_path();
//   auto icse_file = "/models/elevator/elevator.pml";
//   auto file_path = current_path + icse_file;
//   printf("Loading file: %s\n", file_path.c_str());
//   auto loader = std::make_unique<promela_loader>(file_path, tvl);
//   auto automata = loader->getAutomata();
//   mc->startNestedDFS(automata.get(), tvl);
// }