#pragma once
#include <string>
#include <vector>

namespace cxx_utils {
namespace string {
	/**
	 * @brief split the string into pieces of
	 *
	 * @param src
	 * @param ch
	 * @return std::vector<std::string>
	 */
	std::vector<std::string> splits_v1(
	    const std::string& src, const char ch);

	std::vector<std::string_view> splits_v2(
	    const std::string& src, const char ch);

	std::vector<std::string_view> splits_v2_fixed(
	    const std::string& src, const char ch);
}
};
