# CMakeLists.txt

# Step 1: CMake做出约束
cmake_minimum_required(VERSION 3.26)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Step 2: 声明我们的工程
# 什么工程？工程叫什么？版本？
project(string_utils_demo VERSION 0.1 LANGUAGES C CXX)

message(STATUS "Hello World")

# Step 3:
# Build Dependency
# utils库
add_subdirectory(string_utils)

# Step 4
# 创建可执行文件
add_executable(demo demo.cpp)

# Step 5
# 声明库的依赖
target_link_libraries(demo PRIVATE utils)

message(STATUS "Hello World End")
