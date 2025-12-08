#include "banner.h"
#include "string_splits.h"
#include <iostream>

using namespace cxx_utils::example_utils; // Take it easy!

int main() {
	const auto _banner_src = banner_src("Example Will show split of string");
	std::cout << _banner_src;

	auto splits = cxx_utils::string::splits_v1("key=value=int", '=');
	for (const auto& v : splits) {
		std::cout << v << std::endl;
	}

	std::cout << "Bad Call for non holding view: " << std::endl;

	auto splits_view = cxx_utils::string::splits_v2("key=value=int", '=');
	for (const auto& v : splits_view) {
		std::cout << v << std::endl;
	}

	std::string expected_key { "key=value=int" };
	auto splits_view_correct = cxx_utils::string::splits_v2(expected_key, '=');
	for (const auto& v : splits_view_correct) {
		std::cout << v << std::endl;
	}
}
