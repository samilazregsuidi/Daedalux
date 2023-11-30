#include <gtest/gtest.h>
#include <sstream>

#include "../../src/visualizers/traceReport.hpp"
#include "../../src/visualizers/trace.hpp"
#include "../../src/semantic/compositeState.hpp"
#include "../../src/semantic/compositeTransition.hpp"

class TraceReportTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Common setup code that will be called before each test
        report = new traceReport();
    }
    void TearDown() override {
        // Common teardown code that will be called after each test
        delete report;
    }
    traceReport* report;
};

// Test the addBadTrace and getBadTraces methods
TEST_F(TraceReportTest, Test) {
    ASSERT_TRUE(true);
}


// Test the addBadTrace and getBadTraces methods
TEST_F(TraceReportTest, AddBadTraceAndGetBadTraces) {
    std::list<transition*> transList = std::list<transition*>();
    auto state = new compState("test_variable");
    transition* t = new compTransition(state, transList);
    trace *t1 = new trace();
    trace *t2 = new trace();

    t1->addTransition(t);
    t2->addState(state);

    report->addBadTrace(t1);
    report->addBadTrace(t2);

    std::unordered_set<trace*> badTraces = report->getBadTraces();

    ASSERT_EQ(badTraces.size(), 2);
    // ASSERT_TRUE(badTraces.count(t1) > 0);
    // ASSERT_TRUE(badTraces.count(t2) > 0);
}

// Test the addGoodTrace and getGoodTraces methods
TEST_F(TraceReportTest, AddGoodTraceAndGetGoodTraces) {
    std::list<transition*> transList = std::list<transition*>();
    auto state = new compState("test_variable");
    transition* t = new compTransition(state, transList);
    trace* t1 = new trace();
    trace* t2 = new trace();

    t1->addTransition(t);
    t2->addState(state);

    report->addGoodTrace(t1);
    report->addGoodTrace(t2);

    std::unordered_set<trace*> goodTraces = report->getGoodTraces();

    ASSERT_EQ(goodTraces.size(), 2);
    ASSERT_TRUE(goodTraces.count(t1) > 0);
    ASSERT_TRUE(goodTraces.count(t2) > 0);
}



// Test the printCSV method
// TEST_F(TraceReportTest, PrintCSV) {
//     trace t1();
//     trace t2();

//     report->addGoodTrace(t1);
//     report->addBadTrace(t2);

//     std::ostringstream goodTraceStream;
//     std::ostringstream badTraceStream;

//     report->printCSV(goodTraceStream, badTraceStream);

//     std::string expectedGoodTraceCSV = "Trace 1\n";
//     std::string expectedBadTraceCSV = "Trace 2\n";

//     ASSERT_EQ(goodTraceStream.str(), expectedGoodTraceCSV);
//     ASSERT_EQ(badTraceStream.str(), expectedBadTraceCSV);
// }