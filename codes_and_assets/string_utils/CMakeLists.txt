cmake_minimum_required(VERSION 3.25)
project(Week1_string_utils_Projects VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE "Debug")

message(STATUS "Configuring the example utils sources for core")
add_subdirectory(example_utils)
message(STATUS "Configuring the example utils sources for core done")

message(STATUS "Configuring the dirent sources for core")
add_subdirectory(string_splits)
message(STATUS "Configuring the dirent sources for core done")

message(STATUS "Configuring the dirent sources for example")
add_subdirectory(example_usage)
message(STATUS "Configuring the dirent sources for example done")





