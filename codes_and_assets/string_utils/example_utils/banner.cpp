#include "banner.h"
#include "banner/banner_private.h"
#include <cstddef>
#include <sstream>
#include <string_view>

namespace cxx_utils::example_utils {

std::string banner_src(std::string_view view,
                       const BannerAlignment alignment,
                       const char banner_ch) {
	const size_t str_length = view.length();
	const size_t banner_length = example_utils_private::expected_banner_length(view);
	const size_t banner_escape_length = example_utils_private::expected_left_escape(alignment);
	const std::string banners(banner_length, banner_ch);
	const std::string escape_left(banner_escape_length, ' ');

	std::ostringstream banner_stream;
	banner_stream << banners << '\n'
	              << escape_left << view << '\n'
	              << banners << '\n';

	return banner_stream.str();
}

}
