#pragma once
#include <string>
#include <string_view>

namespace cxx_utils {
namespace string {
	enum class TrimPolicy {
		Left,
		Right,
		Both
	};

	std::string
	trim_src(
	    const std::string_view src_view,
	    TrimPolicy policy = TrimPolicy::Both);

	std::string_view
	trim_view(
	    const std::string_view src_view,
	    TrimPolicy policy = TrimPolicy::Both);
}
}
