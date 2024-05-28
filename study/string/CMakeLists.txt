cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(StringProject VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the source directory
add_subdirectory(src)

# Add Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/release-1.11.0.zip
)
FetchContent_MakeAvailable(googletest)

# Enable testing
enable_testing()

# Add the tests directory
add_subdirectory(tests)

# Custom clean-all target to remove all generated files
add_custom_target(clean-all
    COMMAND ${CMAKE_BUILD_TOOL} clean
    COMMAND ${CMAKE_COMMAND} -E rm -rf CMakeFiles CmakeCache.txt
    COMMAND ${CMAKE_COMMAND} -E rm -rf *.cmake Makefile build
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Cleaning all generated files."
)
