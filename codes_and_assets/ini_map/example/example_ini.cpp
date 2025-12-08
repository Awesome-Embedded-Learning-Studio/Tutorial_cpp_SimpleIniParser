#include "ini_parse.h"
#include <cassert>
#include <iostream>
#include <string>

const char* sample = R"ini(
; this is a comment
# also a comment
; top-level keys
topkey = topvalue

[server]
host = example.com
port= 8080
path = "/api/v1/resource" ; inline comment
escaped = "line\nnew"  # another inline comment

[database]
user = dbuser
password = "p@ss;word" ; note semicolon inside quoted value
timeout=30

)ini";

int main() {
	using namespace cxx_utils::ini_parser;

	IniParser parser;
	bool ok = parser.parse({ sample });
	assert(ok);

	for (const auto& sec_pair : parser.data()) {
		std::cout << "[" << sec_pair.first << "]\n";
		for (const auto& kv : sec_pair.second) {
			std::cout << kv.first << " = " << kv.second << "\n";
		}
		std::cout << "\n";
	}

	std::cout << "All tests passed.\n";
	return 0;
}