cmake_minimum_required(VERSION 3.28)
project(serial-frame-pipeline LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
enable_testing()

add_library(sfp_warnings INTERFACE)
target_compile_options(sfp_warnings INTERFACE -Wall -Wextra -Wpedantic)

add_library(sfp_utils src/checksum.cpp src/frame_parser.cpp)
target_include_directories(sfp_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(sfp_utils PRIVATE sfp_warnings)

add_executable(frame_app src/main.cpp)
target_link_libraries(frame_app PRIVATE sfp_utils sfp_warnings)

add_executable(test_checksum test/test_checksum.cpp)
target_link_libraries(test_checksum PRIVATE sfp_utils sfp_warnings)
add_test(NAME checksum_basic COMMAND test_checksum)

add_executable(test_frame_parser test/test_frame_parser.cpp)
target_link_libraries(test_frame_parser PRIVATE sfp_utils sfp_warnings)
add_test(NAME frame_parser_basic COMMAND test_frame_parser)