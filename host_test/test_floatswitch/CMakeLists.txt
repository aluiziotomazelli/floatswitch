# host_test/test_node_state_machine/CMakeLists.txt
cmake_minimum_required(VERSION 3.16)

# Common CMake configuration for all tests in the host_test directory.
include("../extra_dirs_common.cmake")

# Define the project name for this test application.
project(test_floatswitch)

# Set the patterns to extract from the binary
set(EXTRACT_PATTERNS_test_floatswitch "*/floatswitch/src/*.cpp" "*/floatswitch/include/*.hpp")
set(REMOVE_PATTERNS_test_floatswitch "*/floatswitch/include/interfaces/*.hpp")

# Include common coverage logic
include("../coverage_common.cmake")
setup_gtest_coverage(test_floatswitch test_floatswitch.elf)
