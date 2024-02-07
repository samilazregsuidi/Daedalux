#include "../../src/algorithm/explore.hpp"
#include <gtest/gtest.h>

namespace fs = std::filesystem;

class ICSE_TEST : public ::testing::Test {
protected:
  void SetUp() override {}
  void TearDown() override
  {
    // Common teardown code that will be called after each test
  }

  std::unique_ptr<fsm> myFSM;
};

// Test case for the explore function
TEST_F(ICSE_TEST, END_2_END)
{
  const TVL * tvl = nullptr;
  std::string current_path = fs::current_path();
  auto icse_file = "/test_files/basic/flows.pml";
  auto file_path = current_path + icse_file;
  printf("Loading file: %s\n", file_path.c_str());
  auto loader = std::make_unique<promela_loader>(file_path, tvl);
  const fsm* automata = loader->getAutomata().get();
  ltlModelChecker* mc = new ltlModelChecker();
	mc->startNestedDFS(automata, tvl);
	delete mc;
}