## test/CMakeLists.txt
#cmake_minimum_required(VERSION 3.10)
#
## Include FetchContent module
#include(FetchContent)
#
## Download and configure GoogleTest
#FetchContent_Declare(
#  googletest
#  URL https://github.com/google/googletest/archive/refs/tags/release-1.10.0.zip
#)
#FetchContent_MakeAvailable(googletest)
#
## Create the test executable
#add_executable(Ordered_list_test Ordered_list_test.cpp)
#
## Link GoogleTest and the Ordered_list library
#target_link_libraries(Ordered_list_test gtest_main Ordered_list)
#
## Add the test executable to the test suite
#add_test(NAME Ordered_list_test COMMAND Ordered_list_test)

cmake_minimum_required(VERSION 3.10)

# Include FetchContent module
include(FetchContent)

# Download and configure GoogleTest
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
)
FetchContent_MakeAvailable(googletest)

# Use GLOB to find all test files matching the pattern Ordered_list_test*.cpp
file(GLOB TEST_FILES "${CMAKE_CURRENT_SOURCE_DIR}/Ordered_list_test*.cpp")

# Loop through each test file
foreach(TEST_FILE ${TEST_FILES})
  # Extract the test name from the file name
  get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)

  # Create the test executable
  add_executable(${TEST_NAME} ${TEST_FILE})

  # Link GoogleTest and the Ordered_list library
  target_link_libraries(${TEST_NAME} gtest_main Ordered_list)

  # Add the test executable to the test suite
  add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()
