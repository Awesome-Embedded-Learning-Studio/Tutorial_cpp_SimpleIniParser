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
	std::vector<std::string_view> splits(
	    const std::string& src, const char ch);

	std::vector<std::string_view> splits(std::string_view src_view,
	                                     const std::string_view expected_view);
}
};
