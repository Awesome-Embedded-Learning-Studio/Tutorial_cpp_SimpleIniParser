#include "string_trim.h"
#include <cctype>
#include <cstddef>

namespace {

inline bool is_space_char(const char ch) { return std::isspace(ch) != 0; }

} // namespace

namespace cxx_utils::string {

std::string_view trim(const std::string_view sv, const TrimPolicy trim_policy) {
  // 找到第一个非空白字符的index
  // 从后往前找，第一个非空白字符的index
  // [f, e]

  if (sv.empty()) // 输入检查
  {
    return sv;
  }

  size_t end = sv.size();
  size_t start = 0;

  // "   dafdwaefdsa "
  if (trim_policy == TrimPolicy::Both || trim_policy == TrimPolicy::Left) {
    while (start < end && is_space_char(sv[start])) {
      start++;
    }
  }

  // ? 判断是不是空白字符
  if (trim_policy == TrimPolicy::Both || trim_policy == TrimPolicy::Right) {
    while (end > start && is_space_char(sv[end - 1])) {
      end--;
    }
  }

  return sv.substr(start, end - start);
}

} // namespace cxx_utils::string
