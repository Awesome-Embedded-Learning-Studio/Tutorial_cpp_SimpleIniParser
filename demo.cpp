#include "string_splits.h"
#include "string_trim.h"
#include <iostream>

// CMake

// demo -> split
// 可执行文件 demo
// split ?
// 可复用的库 Charliechen114514《深入理解C/C++编译技术》
// 静态库 动态库
// string_utils/string_splits.cpp -> libuitls.a
// demo -> libuitls.a

// 正确性
// cmake 构建系统 几千个文件 上万个文件

// trim
int main() {
  using namespace cxx_utils::string;
  std::string kv{"key=value=int"};
  auto results = splits_v2_fix(kv, '=');

  for (const auto &each_result : results) {
    std::cout << each_result << std::endl;
  }

  std::string wait_trim = "   dfniouafdhionhdoiwans   ";

  const auto result = trim(wait_trim);

  std::cout << result << std::endl;
}