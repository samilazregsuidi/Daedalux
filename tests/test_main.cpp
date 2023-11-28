#include <gtest/gtest.h>

// Add your test files here
#include "symbols/test_symTable.cpp"
#include "symbols/test_intSymNode.cpp"
#include "symbols/test_bitSymNode.cpp"
#include "symbols/test_varSymNode.cpp"
#include "symbols/test_symbol.cpp"

// #include "test_elementStack.cpp"

int main(int argc, char **argv) {
    // Initialize Google Test framework
    ::testing::InitGoogleTest(&argc, argv);

    // Run the tests
    return RUN_ALL_TESTS();
}
