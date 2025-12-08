#pragma once
#include "../banner.h"
#include <cstddef>
#include <string_view>
namespace cxx_utils {
namespace example_utils_private {

	static constexpr const size_t LENGTH_ESCAPE = 5;

	std::size_t
	expected_banner_length(const std::string_view v);

	std::size_t
	expected_left_escape(const example_utils::BannerAlignment alignment);

}
}
