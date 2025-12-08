#include "banner_private.h"

namespace cxx_utils::example_utils_private {
std::size_t
expected_banner_length(const std::string_view v) {
	return 2 * LENGTH_ESCAPE + v.length();
}

std::size_t
expected_left_escape(const example_utils::BannerAlignment alignment) {
	switch (alignment) {
	case example_utils::BannerAlignment::LEFT:
		return 0;
	case example_utils::BannerAlignment::CENTER:
		return LENGTH_ESCAPE;
	case example_utils::BannerAlignment::RIGHT:
		return 2 * LENGTH_ESCAPE;
	}
	return LENGTH_ESCAPE;
}

}
