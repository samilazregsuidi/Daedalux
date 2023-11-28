cmake_minimum_required(VERSION 3.14)

include(FetchContent)

# Fetch and include GoogleTest
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.11.0
)

FetchContent_MakeAvailable(googletest)

# Include the CTest module to enable testing
include(CTest)

# List your test source files
set(TEST_SOURCES
    test_main.cpp
    # test_elementStack.cpp
    # test_intSymNode.cpp
    # test_symTable.cpp
    # Add more test files as needed
)

# Create an executable for the tests
add_executable(test_suite ${TEST_SOURCES})

# Link your test executable with the libraries it depends on
target_link_libraries(test_suite
    PRIVATE
    daedalux_lib
    gtest
    gtest_main
)

# Use gtest_discover_tests for test discovery
include(GoogleTest)
gtest_discover_tests(test_suite)

# Add a test
add_test(NAME DeadaluxTests COMMAND test_suite)

# # remove the test driver source file
# set (TestsToRun ${Tests})
# remove (TestsToRun CommonCxxTests.cxx)

# # Add all the ADD_TEST for each test
# foreach (test ${TestsToRun})
#   get_filename_component (TName ${test} NAME_WE)
#   add_test (NAME ${TName} COMMAND CommonCxxTests ${TName})
# endforeach ()




# target_link_libraries(test_symtable
#   PRIVATE
#   GTest::GTest
#   daedalux_lib
# )

# target_link_libraries(test_intSymNode
#   PRIVATE
#   GTest::GTest
#   daedalux_lib
# )


# add_test(IntSymNode_gtests test_intSymNode)

# include(GoogleTest)
# gtest_discover_tests(test_symtable)
# gtest_discover_tests(test_intSymNode)