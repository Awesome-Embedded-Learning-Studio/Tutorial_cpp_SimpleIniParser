cmake_minimum_required(VERSION 3.25)
project(Week1_ini_parse_Projects VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE "Debug")

message(STATUS "Configuring the example utils sources for core")
add_subdirectory(example_utils)
message(STATUS "Configuring the example utils sources for core done")

add_subdirectory(string_splits)
add_subdirectory(ini_parse)

add_subdirectory(example)
add_subdirectory(test)

