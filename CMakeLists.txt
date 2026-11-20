cmake_minimum_required(VERSION 3.28)
project(serial-frame-pipeline LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(frame_app src/main.cpp)
target_compile_options(frame_app PRIVATE -Wall -Wextra -Wpedantic)