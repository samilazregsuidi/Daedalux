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

#include "test_ltl_creator.cpp"


// VISUALIZER - TESTS
#include "vizualizers/test_trace.cpp"
#include "vizualizers/test_traceReport.cpp"
#include "vizualizers/test_stateToGraphViz.cpp"

// #include "test_transition.cpp"
#include "test_state.cpp"
#include "test_elementStack.cpp"
#include "test_promela_loader.cpp"
#include "test_specification_writer.cpp"

// SEMANTIC - TESTS
#include "semantic/test_init_state.cpp"
#include "semantic/test_executables.cpp"

// INTEGRATION TESTS
#include "integration_tests/test_mutantgeneration.cpp"
#include "integration_tests/test_ltlmodelChecker.cpp"
#include "integration_tests/test_most_similar_trans.cpp"
#include "integration_tests/test_trace_generator.cpp"
#include "integration_tests/test_bisimulation.cpp"

#include "integration_tests/test_formula_creator.cpp"
#include "integration_tests/test_mutant_handling.cpp"

// Add your test files here



int main(int argc, char **argv) {
    // Initialize Google Test framework
    ::testing::InitGoogleTest(&argc, argv);

    std::cout << "Running main() from test_main.cpp\n";

    // Run the tests
    return RUN_ALL_TESTS();
}
