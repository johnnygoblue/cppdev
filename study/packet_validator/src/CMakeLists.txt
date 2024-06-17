add_library(packet_validator STATIC packet_validator.cpp)

target_include_directories(packet_validator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(packet_validator pthread)
