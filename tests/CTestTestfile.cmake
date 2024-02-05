# CMake generated Testfile for 
# Source directory: /home/slazreg/Work/Research/Daedalux/tests
# Build directory: /home/slazreg/Work/Research/Daedalux/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
include("/home/slazreg/Work/Research/Daedalux/tests/test_suite[1]_include.cmake")
add_test(DeadaluxTests "/home/slazreg/Work/Research/Daedalux/tests/test_suite")
set_tests_properties(DeadaluxTests PROPERTIES  _BACKTRACE_TRIPLES "/home/slazreg/Work/Research/Daedalux/tests/CMakeLists.txt;39;add_test;/home/slazreg/Work/Research/Daedalux/tests/CMakeLists.txt;0;")
subdirs("../_deps/googletest-build")
