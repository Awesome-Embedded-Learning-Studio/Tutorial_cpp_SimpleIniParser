#include "string_utils/string_splits.h"
#include <iostream>

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
}