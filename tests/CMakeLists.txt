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

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../test_files DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../models DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# List your test source files
set(TEST_SOURCES
    test_main.cpp
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