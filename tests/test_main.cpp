#include <gtest/gtest.h>

#include "integration_tests/test_model_analyzer.cpp"


// Add your test files here
//#include "symbols/test_symTable.cpp"
//#include "symbols/test_intSymNode.cpp"
//#include "symbols/test_bitSymNode.cpp"
//#include "symbols/test_varSymNode.cpp"
//#include "symbols/test_symbol.cpp"

//#include "test_fsm.cpp"
//#include "test_fsmEdge.cpp"
//#include "test_fsmNode.cpp"

//#include "test_ADDutils.cpp"

<<<<<<< HEAD
<<<<<<< HEAD
// VISUALIZER - TESTS
#include "vizualizers/test_trace.cpp"
#include "vizualizers/test_traceReport.cpp"
#include "vizualizers/test_stateToGraphViz.cpp"

// #include "test_transition.cpp"
#include "test_state.cpp"
#include "test_elementStack.cpp"
#include "test_promela_loader.cpp"
#include "test_specification_writer.cpp"
=======
//#include "vizualizers/test_trace.cpp"
//#include "vizualizers/test_traceReport.cpp"
//#include "vizualizers/test_stateToGraphViz.cpp"
=======
//#include "vizualizers/test_trace.cpp"
//#include "vizualizers/test_traceReport.cpp"
//#include "vizualizers/test_stateToGraphViz.cpp"


#include "test_symbol_table.cpp"

// #include "test_transition.cpp"
//#include "test_state.cpp"

//#include "test_elementStack.cpp"
>>>>>>> main


<<<<<<< HEAD
#include "test_symbol_table.cpp"

// #include "test_transition.cpp"
//#include "test_state.cpp"

//#include "test_elementStack.cpp"
>>>>>>> main

// SEMANTIC - TESTS
#include "semantic/test_init_state.cpp"

<<<<<<< HEAD
// INTEGRATION TESTS
#include "integration_tests/test_mutantgeneration.cpp"
#include "integration_tests/test_ltlmodelChecker.cpp"
#include "integration_tests/test_most_similar_trans.cpp"
#include "integration_tests/test_trace_generator.cpp"
#include "integration_tests/test_bisimulation.cpp"
#include "integration_tests/test_mutant_handling.cpp"

// FORMULA - TESTS
#include "formulas/test_formula_creator.cpp"
#include "formulas/test_ltl_creator.cpp"
#include "formulas/test_formula.cpp"
#include "formulas/test_spinRunner.cpp"
=======
//#include "integration_tests/test_mutantgeneration.cpp"

//#include "integration_tests/test_dfs.cpp"
>>>>>>> main
=======
//#include "integration_tests/test_mutantgeneration.cpp"

//#include "integration_tests/test_dfs.cpp"
>>>>>>> main

int main(int argc, char **argv) {
    // Initialize Google Test framework
    ::testing::InitGoogleTest(&argc, argv);

    std::cout << "Running main() from test_main.cpp\n";

    // Run the tests
    return RUN_ALL_TESTS();
}
