#include <gtest/gtest.h>

// Add your test files here
#include "symbols/test_symTable.cpp"
#include "symbols/test_intSymNode.cpp"
#include "symbols/test_bitSymNode.cpp"
#include "symbols/test_varSymNode.cpp"
#include "symbols/test_symbol.cpp"

#include "test_fsm.cpp"
#include "test_fsmEdge.cpp"
#include "test_fsmNode.cpp"

#include "test_ADDutils.cpp"

// VIZUALIZER - TESTS
#include "vizualizers/test_trace.cpp"
#include "vizualizers/test_traceReport.cpp"
#include "vizualizers/test_stateToGraphViz.cpp"


// #include "test_transition.cpp"
#include "test_state.cpp"

#include "test_elementStack.cpp"

// #include "test_promela_loader.cpp"

// INTEGRATION TESTS
#include "integration_tests/test_mutantgeneration.cpp"
#include "integration_tests/test_dfs.cpp"
#include "integration_tests/test_most_similar_trans.cpp"


int main(int argc, char **argv) {
    // Initialize Google Test framework
    ::testing::InitGoogleTest(&argc, argv);

    std::cout << "Running main() from test_main.cpp\n";

    // Run the tests
    return RUN_ALL_TESTS();
}
