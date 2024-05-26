cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(Calc VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Define target names
set(SRC_TARGET_NAME calc)
set(TEST_TARGET_NAME runTests)

# Set the installation prefix to a local directory
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/../install)

#Add subdirectories
add_subdirectory(src)
add_subdirectory(tests)

# Custom clean target
add_custom_target(clean-all
    COMMAND ${CMAKE_BUILD_TOOL} clean
    COMMAND ${CMAKE_COMMAND} -E rm -rf CMakeFiles CMakeCache.txt
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/../bin/*
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Cleaning all generated files."
)
