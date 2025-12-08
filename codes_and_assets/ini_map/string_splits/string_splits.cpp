#include "string_splits.h"
#include <algorithm>
#include <cstddef>
#include <string>
#include <string_view>

namespace cxx_utils::string {

// Post Clean Codes
std::vector<std::string_view> splits(
    const std::string& src, const char ch) {

	std::vector<std::string_view> results;
	if (src.empty()) {
		return results;
	}

	std::string_view src_view(src);

	const size_t delim_count = std::count(src.begin(), src.end(), ch);
	results.reserve(delim_count * 2 + 1);

	size_t last_index = 0;
	size_t current_positions = src.find(ch, last_index);

	while (current_positions != std::string::npos) {

		results.emplace_back(
		    src_view.substr(last_index, current_positions - last_index));

		results.emplace_back(
		    src_view.substr(current_positions, 1));

		last_index = current_positions + 1;
		current_positions = src.find(ch, last_index);
	}

	results.emplace_back(src_view.substr(last_index));

	return results;
}

std::vector<std::string_view> splits(std::string_view src_view,
                                     const std::string_view expected_view) {
	std::vector<std::string_view> results;
	if (src_view.empty()) {
		return results;
	}
	// dont allocate prev

	size_t last_index = 0;
	size_t current_positions = src_view.find(expected_view, last_index);

	while (current_positions != std::string::npos) {
		const size_t expected_length = expected_view.length();
		results.emplace_back(
		    src_view.substr(last_index, current_positions - last_index));

		results.emplace_back(
		    src_view.substr(current_positions, expected_length));

		last_index = current_positions + expected_length;
		current_positions = src_view.find(expected_view, last_index);
	}

	results.emplace_back(src_view.substr(last_index));
	return results;
}

}
