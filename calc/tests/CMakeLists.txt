# Add Google Test using FetchContent
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/release-1.11.0.zip
)
FetchContent_MakeAvailable(googletest)

# Enable testing
enable_testing()

# Add tests directory
add_executable(${TEST_TARGET_NAME} test_calc.cpp)

# Add include directories
target_include_directories(${TEST_TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)

target_link_libraries(${TEST_TARGET_NAME} GTest::gtest_main)

# discover tests
include(GoogleTest)
gtest_discover_tests(${TEST_TARGET_NAME})

install(TARGETS ${TEST_TARGET_NAME} DESTINATION ${CMAKE_BINARY_DIR}/../bin)
