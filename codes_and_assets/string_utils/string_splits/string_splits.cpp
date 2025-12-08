#include "string_splits.h"
#include <algorithm>
#include <cstddef>
#include <string>
#include <string_view>

namespace cxx_utils::string {

std::vector<std::string> splits_v1(
    const std::string& src, const char ch) {
	std::vector<std::string> results; // result containers
	static constexpr const int start_pos = 0;

	auto current_positions = src.find(ch, start_pos);
	const size_t str_sz = src.size();
	size_t last_index = start_pos;

	while (current_positions != std::string::npos) {
		results.emplace_back(src.substr(last_index, current_positions - last_index));
		results.emplace_back(1, ch);
		last_index = current_positions + 1;
		current_positions = src.find(ch, last_index);
	}

	results.emplace_back(src.substr(last_index));
	return results;
}

std::vector<std::string_view> splits_v2(
    const std::string& src, const char ch) {
	std::vector<std::string_view> results; // result containers

	if (src.empty()) {
		return results;
	}

	static constexpr const int start_pos = 0;
	const size_t delim_count = std::count(src.begin(), src.end(), ch);
	results.reserve(delim_count * 2 + 1);

	auto current_positions = src.find(ch, start_pos);
	const size_t str_sz = src.size();
	size_t last_index = start_pos;

	while (current_positions != std::string::npos) {
		// Errors here, src returns a brand new string
		// view will erase it
		results.emplace_back(src.substr(last_index, current_positions - last_index));
		results.emplace_back(src.substr(current_positions, 1));
		last_index = current_positions + 1;
		current_positions = src.find(ch, current_positions + 1);
	}

	results.emplace_back(src.substr(last_index));
	return results;
}

std::vector<std::string_view> splits_v2_fixed(
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

}
