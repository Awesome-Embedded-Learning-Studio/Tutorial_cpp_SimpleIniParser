#pragma once
#include <string>      // dependency: string
#include <string_view> // dependency: string_view

namespace cxx_utils {
namespace string {

enum class TrimPolicy { Left, Right, Both };

std::string_view trim(const std::string_view sv,
                      const TrimPolicy trim_policy = TrimPolicy::Both);

} // namespace string
} // namespace cxx_utils
