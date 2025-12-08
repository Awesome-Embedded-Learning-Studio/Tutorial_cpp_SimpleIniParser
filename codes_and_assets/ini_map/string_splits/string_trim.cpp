#include "string_trim.h"
#include <cstddef>

namespace {

inline bool is_space_char(char ch) {
	return std::isspace(static_cast<unsigned char>(ch)) != 0;
}

}

namespace cxx_utils {
namespace string {
	std::string
	trim_src(
	    const std::string_view src_view,
	    TrimPolicy policy) {
		return std::string { trim_view(src_view, policy) };
	}

	std::string_view
	trim_view(
	    const std::string_view src_view,
	    TrimPolicy policy) {
		// if the src_view empty
		if (src_view.empty()) {
			return src_view;
		}

		size_t end = src_view.size();
		size_t start = 0;

		if (policy == TrimPolicy::Both || policy == TrimPolicy::Left) {
			while (start < end && is_space_char(src_view[start])) {
				start++;
			}
		}

		if (policy == TrimPolicy::Both || policy == TrimPolicy::Right) {
			while (end > start && is_space_char(src_view[end - 1])) {
				end--;
			}
		}

		return src_view.substr(start, end - start);
	}
}
}
