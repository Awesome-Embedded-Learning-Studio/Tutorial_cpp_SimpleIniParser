message("Enter to build string utils")

# 库
# 二进制交出去
# 头文件
add_library(utils string_splits.cpp string_trim.cpp)

target_include_directories(utils PUBLIC .) # 交给demo使用

message("Enter to build string utils finished")
