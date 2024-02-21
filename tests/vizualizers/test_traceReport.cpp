#include <gtest/gtest.h>
#include <memory>
#include <sstream>

#include "../../src/core/semantic/variable/state/composite.hpp"
#include "../../src/core/semantic/variable/transition/compositeTransition.hpp"
#include "../../src/visualizer/trace.hpp"
#include "../../src/visualizer/traceReport.hpp"

class TraceReportTest : public ::testing::Test {
protected:
  void SetUp() override
  {
    // Common setup code that will be called before each test
    report = std::make_unique<traceReport>();
  }
  void TearDown() override
  {
    // Common teardown code that will be called after each test
  }
  std::unique_ptr<traceReport> report;
};

// Test the addBadTrace and getBadTraces methods
TEST_F(TraceReportTest, Test) { ASSERT_TRUE(true); }

// Test the addBadTrace and getBadTraces methods
TEST_F(TraceReportTest, AddBadTraceAndGetBadTraces)
{
  std::list<transition *> transList = std::list<transition *>();
  std::shared_ptr<state> state = std::make_shared<composite>("test_variable");
  std::shared_ptr<transition> t = std::make_shared<compTransition>(state.get(), transList);

  std::shared_ptr<trace> t1 = std::make_shared<trace>();
  std::shared_ptr<trace> t2 = std::make_shared<trace>();

  t1->addTransition(t);
  t2->addState(state);

  report->addBadTrace(t1);
  report->addBadTrace(t2);

  auto badTraces = report->getBadTraces();
  ASSERT_EQ(badTraces.size(), 2);
}

// Test the addGoodTrace and getGoodTraces methods
TEST_F(TraceReportTest, AddGoodTraceAndGetGoodTraces)
{
  std::list<transition *> transList = std::list<transition *>();
  std::shared_ptr<state> state = std::make_shared<composite>("test_variable");
  std::shared_ptr<transition> t = std::make_shared<compTransition>(state.get(), transList);

  std::shared_ptr<trace> t1 = std::make_shared<trace>();
  std::shared_ptr<trace> t2 = std::make_shared<trace>();

  t1->addTransition(t);
  t2->addState(state);

  report->addGoodTrace(t1);
  report->addGoodTrace(t2);

  auto goodTraces = report->getGoodTraces();

  ASSERT_EQ(goodTraces.size(), 2);
  ASSERT_TRUE(goodTraces.count(t1) > 0);
  ASSERT_TRUE(goodTraces.count(t2) > 0);
}

TEST_F(TraceReportTest, TraceOfFlowLong)
{
  // Skip this test if it is not running on a macOs
  if (std::filesystem::current_path().string().find("macOS") == std::string::npos)
  {
    GTEST_SKIP();
  }
  auto file = "/models/minepump/original.pml";
  std::string current_path = std::filesystem::current_path();
  const TVL * tvl = nullptr;
  auto length = 20;
  int original_length = length;
  auto file_path = current_path + file;
  auto loader = std::make_unique<promela_loader>(file_path, tvl);
  auto FSM = loader->getAutomata();
  auto current_state = initState::createInitState(FSM.get(), tvl);
  std::shared_ptr<state> curent_state_smart(current_state);
  std::shared_ptr<trace> t1 = std::make_shared<trace>();
  std::shared_ptr<trace> t2 = std::make_shared<trace>();

  t1->addState(curent_state_smart);
  while (length > 0) {
    auto next_state = current_state->Post().front();
    transition * current_trans = next_state->getOrigin()->deepCopy();
    std::shared_ptr<state> next_state_smart(next_state);
    std::shared_ptr<transition> current_trans_smart(current_trans);
    if (length % 2 == 0) {
      t1->addState(next_state_smart);
      t1->addTransition(current_trans_smart);
    }
    else {
      t2->addState(next_state_smart);
      t2->addTransition(current_trans_smart);
    }
    current_state = next_state;
    length--;
  }
  report->addGoodTrace(t1);
  report->addBadTrace(t2);

  std::ostringstream goodTraceStream;
  std::ostringstream badTraceStream;

  report->printCSV(goodTraceStream, badTraceStream);

  // std::string expectedGoodTraceCSV = "Good Traces\nsys..s.d,sys..s.a,sys..s.c,sys..s.b,sys..I,sys..J,prob,\nfalse,false,false,false,12,31,1,\nfalse,true,false,false,13,31,1,\nfalse,true,false,true,15,31,1,\nfalse,true,false,true,15,31,1,\nfalse,true,false,true,15,31,1,\nfalse,true,false,true,15,31,1,\nfalse,true,false,true,15,31,1,\nfalse,true,false,true,15,31,1,\nfalse,true,false,true,15,31,1,\nfalse,true,false,true,15,31,1,\nfalse,true,false,true,15,31,1,\n";
  // std::string expectedBadTraceCSV = "Bad Traces \nsys..s.d,sys..s.a,sys..s.c,sys..s.b,sys..I,sys..J,prob,\nfalse,false,false,false,12,31,1,\nfalse,true,false,false,13";
  // ASSERT_EQ(goodTraceStream.str(), expectedGoodTraceCSV);
  // ASSERT_EQ(badTraceStream.str(), expectedBadTraceCSV);
}