#pragma once
#include <string> // dependency: string
#include <string_view>
#include <vector> // dependency: vector

// interface of string splits
namespace cxx_utils {
namespace string {

// 存在改进空间的
std::vector<std::string> splits(const std::string &src, const char ch);

// 迭代
// string_view
std::vector<std::string_view> splits_v2(const std::string &src, const char ch);

// fix v2
std::vector<std::string_view> splits_v2_fix(std::string_view src,
                                            const char ch);

} // namespace string
} // namespace cxx_utils
