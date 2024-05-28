# Add the library
add_library(StringLib String.cpp)

# Include directories
target_include_directories(StringLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

