# Find all test files with the prefix StringTest
file(GLOB TEST_SOURCES "StringTest*.cpp")

# Iterate over each test source file and create an executable and add it to CTest
foreach(TEST_SOURCE ${TEST_SOURCES})
    # Get the filename without the directory
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)

    # Create the test executable
    add_executable(${TEST_NAME} ${TEST_SOURCE})

    # Link the String library and Google Test libraries
    target_link_libraries(${TEST_NAME} PRIVATE StringLib GTest::gtest_main)

    # Add the test to CTest
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

    # Include the src directory for header files
    target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)
endforeach()
