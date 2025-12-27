#include "string_splits.h"
#include <algorithm>
#include <cstddef>
#include <string_view>

namespace cxx_utils {
namespace string {

std::vector<std::string> splits(const std::string &src, const char ch) {
  static constexpr const int START_POS = 0;
  auto current_equal_posotion = src.find(ch, START_POS);
  size_t last_index = START_POS;

  // our result
  std::vector<std::string> result;

  while (current_equal_posotion != std::string::npos) {
    result.emplace_back(
        src.substr(last_index, current_equal_posotion - last_index));
    result.emplace_back(1, ch);
    last_index = current_equal_posotion + 1;
    current_equal_posotion = src.find(ch, last_index);
  }

  // value
  result.emplace_back(src.substr(last_index));

  return result;
}

// bug
std::vector<std::string_view> splits_v2(const std::string &src, const char ch) {
  static constexpr const int START_POS = 0;
  auto current_equal_posotion = src.find(ch, START_POS);
  size_t last_index = START_POS;
  std::vector<std::string_view> result; // reserve
  const size_t delim_cnt = std::count(src.begin(), src.end(), ch);
  result.reserve(2 * delim_cnt + 1);

  while (current_equal_posotion != std::string::npos) {
    result.emplace_back(

        // std::string 临时的
        src.substr(last_index, current_equal_posotion - last_index)

    );
    result.emplace_back(src.substr(current_equal_posotion, 1));
    last_index = current_equal_posotion + 1;
    current_equal_posotion = src.find(ch, last_index);
  }

  // value
  result.emplace_back(src.substr(last_index));

  return result;
}

// name
std::vector<std::string_view> splits_v2_fix(std::string_view src,
                                            const char ch) {
  static constexpr const int START_POS = 0;
  auto current_equal_posotion = src.find(ch, START_POS);
  size_t last_index = START_POS;
  std::vector<std::string_view> result; // reserve
  const size_t delim_cnt = std::count(src.begin(), src.end(), ch);
  result.reserve(2 * delim_cnt + 1);

  while (current_equal_posotion != std::string::npos) {
    result.emplace_back(
        src.substr(last_index, current_equal_posotion - last_index));
    result.emplace_back(src.substr(current_equal_posotion, 1));
    last_index = current_equal_posotion + 1;
    current_equal_posotion = src.find(ch, last_index);
  }

  // value
  result.emplace_back(src.substr(last_index));

  return result;
}

} // namespace string
} // namespace cxx_utils
