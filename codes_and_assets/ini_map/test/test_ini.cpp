#include "ini_parse.h"
#include <cassert>
#include <iostream>
#include <string>

using namespace cxx_utils::ini_parser;

// 测试计数器
int tests_passed = 0;
int tests_failed = 0;

#define TEST_CASE(name) \
	std::cout << "\n=== " << name << " ===" << std::endl;

#define ASSERT_EQ(actual, expected, msg)                                                                        \
	do {                                                                                                        \
		if ((actual) == (expected)) {                                                                           \
			tests_passed++;                                                                                     \
			std::cout << "✓ " << msg << std::endl;                                                              \
		} else {                                                                                                \
			tests_failed++;                                                                                     \
			std::cout << "✗ " << msg << "\n  Expected: " << (expected) << "\n  Got: " << (actual) << std::endl; \
		}                                                                                                       \
	} while (0)

#define ASSERT_TRUE(condition, msg)                                         \
	do {                                                                    \
		if (condition) {                                                    \
			tests_passed++;                                                 \
			std::cout << "✓ " << msg << std::endl;                          \
		} else {                                                            \
			tests_failed++;                                                 \
			std::cout << "✗ " << msg << " (condition failed)" << std::endl; \
		}                                                                   \
	} while (0)

#define ASSERT_FALSE(condition, msg)                                      \
	do {                                                                  \
		if (!(condition)) {                                               \
			tests_passed++;                                               \
			std::cout << "✓ " << msg << std::endl;                        \
		} else {                                                          \
			tests_failed++;                                               \
			std::cout << "✗ " << msg << " (expected false)" << std::endl; \
		}                                                                 \
	} while (0)

void test_basic_parsing() {
	TEST_CASE("Basic Parsing");

	const char* ini = R"ini(
key1 = value1
key2 = value2
[section1]
key3 = value3
)ini";

	IniParser parser;
	bool ok = parser.parse(ini);

	ASSERT_TRUE(ok, "Parse should succeed");
	ASSERT_EQ(parser.get("", "key1").value_or(""), "value1", "Top-level key1");
	ASSERT_EQ(parser.get("", "key2").value_or(""), "value2", "Top-level key2");
	ASSERT_EQ(parser.get("section1", "key3").value_or(""), "value3", "Section1 key3");
}

void test_comments() {
	TEST_CASE("Comment Handling");

	const char* ini = R"ini(
; semicolon comment
# hash comment
key1 = value1 ; inline comment
key2 = value2 # inline hash comment
key3 = "value;with;semicolons" ; this is a comment
key4 = "value#with#hashes" # comment
)ini";

	IniParser parser;
	parser.parse(ini);

	ASSERT_EQ(parser.get("", "key1").value_or(""), "value1", "Value before inline semicolon comment");
	ASSERT_EQ(parser.get("", "key2").value_or(""), "value2", "Value before inline hash comment");
	ASSERT_EQ(parser.get("", "key3").value_or(""), "value;with;semicolons", "Quoted semicolons preserved");
	ASSERT_EQ(parser.get("", "key4").value_or(""), "value#with#hashes", "Quoted hashes preserved");
}

void test_sections() {
	TEST_CASE("Section Handling");

	const char* ini = R"ini(
toplevel = top
[section1]
key1 = val1
[section2]
key2 = val2
[section1]
key3 = val3
)ini";

	IniParser parser;
	parser.parse(ini);

	ASSERT_EQ(parser.get("", "toplevel").value_or(""), "top", "Top-level key");
	ASSERT_EQ(parser.get("section1", "key1").value_or(""), "val1", "Section1 first key");
	ASSERT_EQ(parser.get("section1", "key3").value_or(""), "val3", "Section1 second occurrence");
	ASSERT_EQ(parser.get("section2", "key2").value_or(""), "val2", "Section2 key");
}

void test_quoted_values() {
	TEST_CASE("Quoted Values");

	const char* ini = R"ini(
single = "quoted value"
double = "value with spaces"
empty = ""
withequals = "key=value"
mixed = "start"middle"end"
password = "p@ss;word"
)ini";

	IniParser parser;
	parser.parse(ini);

	ASSERT_EQ(parser.get("", "single").value_or(""), "quoted value", "Single quoted");
	ASSERT_EQ(parser.get("", "double").value_or(""), "value with spaces", "Spaces in quotes");
	ASSERT_EQ(parser.get("", "empty").value_or(""), "", "Empty quotes");
	ASSERT_EQ(parser.get("", "withequals").value_or(""), "key=value", "Equals in quotes");
	ASSERT_EQ(parser.get("", "password").value_or(""), "p@ss;word", "Special chars in quotes");
}

void test_escape_sequences() {
	TEST_CASE("Escape Sequences");

	const char* ini = R"ini(
newline = "line1\nline2"
tab = "col1\tcol2"
backslash = "path\\to\\file"
quote = "say \"hello\""
)ini";

	IniParser parser;
	parser.parse(ini);

	ASSERT_EQ(parser.get("", "newline").value_or(""), "line1\nline2", "Newline escape");
	ASSERT_EQ(parser.get("", "tab").value_or(""), "col1\tcol2", "Tab escape");
	ASSERT_EQ(parser.get("", "backslash").value_or(""), "path\\to\\file", "Backslash escape");
	ASSERT_EQ(parser.get("", "quote").value_or(""), "say \"hello\"", "Quote escape");
}

void test_whitespace_handling() {
	TEST_CASE("Whitespace Handling");

	const char* ini = R"ini(
key1=value1
key2 = value2
key3  =  value3
  key4  =  value4  
[  section1  ]
  key5 = value5  
)ini";

	IniParser parser;
	parser.parse(ini);

	ASSERT_EQ(parser.get("", "key1").value_or(""), "value1", "No spaces");
	ASSERT_EQ(parser.get("", "key2").value_or(""), "value2", "Normal spaces");
	ASSERT_EQ(parser.get("", "key3").value_or(""), "value3", "Multiple spaces");
	ASSERT_EQ(parser.get("", "key4").value_or(""), "value4", "Leading/trailing spaces");
	ASSERT_EQ(parser.get("section1", "key5").value_or(""), "value5", "Section with spaces");
}

void test_empty_values() {
	TEST_CASE("Empty Values");

	const char* ini = R"ini(
empty1 =
empty2 = 
empty3 =   
quoted_empty = ""
)ini";

	IniParser parser;
	parser.parse(ini);

	ASSERT_EQ(parser.get("", "empty1").value_or("NONE"), "", "Empty value 1");
	ASSERT_EQ(parser.get("", "empty2").value_or("NONE"), "", "Empty value 2");
	ASSERT_EQ(parser.get("", "empty3").value_or("NONE"), "", "Empty value 3");
	ASSERT_EQ(parser.get("", "quoted_empty").value_or("NONE"), "", "Quoted empty");
}

void test_multiline_and_continuation() {
	TEST_CASE("Multiple Values with Equals");

	const char* ini = R"ini(
equation = a=b=c
url = http://example.com?param=value&other=data
)ini";

	IniParser parser;
	parser.parse(ini);

	ASSERT_EQ(parser.get("", "equation").value_or(""), "a=b=c", "Multiple equals signs");
	ASSERT_EQ(parser.get("", "url").value_or(""), "http://example.com?param=value&other=data", "URL with params");
}

void test_edge_cases() {
	TEST_CASE("Edge Cases");

	// Empty file
	{
		IniParser parser;
		bool ok = parser.parse("");
		ASSERT_TRUE(ok, "Empty file should parse");
	}

	// Only comments
	{
		const char* ini = R"ini(
; comment 1
# comment 2
)ini";
		IniParser parser;
		bool ok = parser.parse(ini);
		ASSERT_TRUE(ok, "Comments-only file should parse");
	}

	// Only whitespace
	{
		const char* ini = "   \n  \n   \n";
		IniParser parser;
		bool ok = parser.parse(ini);
		ASSERT_TRUE(ok, "Whitespace-only file should parse");
	}

	// Section at end
	{
		const char* ini = R"ini(
key = value
[section]
)ini";
		IniParser parser;
		parser.parse(ini);
		ASSERT_EQ(parser.get("", "key").value_or(""), "value", "Key before empty section");
	}
}

void test_invalid_syntax() {
	TEST_CASE("Invalid Syntax");

	// Unclosed section - should still parse but might fail
	{
		const char* ini = "[unclosed\nkey=value";
		IniParser parser;
		parser.parse(ini);
		// Just ensure it doesn't crash
		ASSERT_TRUE(true, "Unclosed section handled");
	}

	// Missing equals
	{
		const char* ini = "invalidline\nkey=value";
		IniParser parser;
		parser.parse(ini);
		ASSERT_EQ(parser.get("", "key").value_or(""), "value", "Valid line after invalid");
	}
}

void test_special_characters() {
	TEST_CASE("Special Characters");

	const char* ini = R"ini(
unicode = "Hello世界"
symbols = "!@#$%^&*()"
brackets = "()[]{}"
path = "/usr/local/bin"
email = "user@example.com"
)ini";

	IniParser parser;
	parser.parse(ini);

	ASSERT_EQ(parser.get("", "unicode").value_or(""), "Hello世界", "Unicode characters");
	ASSERT_EQ(parser.get("", "symbols").value_or(""), "!@#$%^&*()", "Symbol characters");
	ASSERT_EQ(parser.get("", "brackets").value_or(""), "()[]{}", "Bracket characters");
	ASSERT_EQ(parser.get("", "path").value_or(""), "/usr/local/bin", "Path with slashes");
	ASSERT_EQ(parser.get("", "email").value_or(""), "user@example.com", "Email address");
}

void test_section_ordering() {
	TEST_CASE("Section Ordering and Overwriting");

	const char* ini = R"ini(
topkey = topvalue
[database]
user = dbuser
[server]
host = example.com
)ini";

	IniParser parser;
	parser.parse(ini);

	ASSERT_EQ(parser.get("", "topkey").value_or(""), "topvalue", "Top-level before sections");
	ASSERT_EQ(parser.get("database", "user").value_or(""), "dbuser", "Database section");
	ASSERT_EQ(parser.get("server", "host").value_or(""), "example.com", "Server section");
	ASSERT_FALSE(parser.get("database", "topkey").has_value(), "topkey NOT in database section");
}

void test_complex_real_world() {
	TEST_CASE("Complex Real-World Example");

	const char* ini = R"ini(
; Application Configuration
app_name = MyApp
version = 1.2.3

[server]
host = 0.0.0.0
port = 8080
ssl = true
cert_path = "/etc/ssl/certs/server.crt"
key_path = "/etc/ssl/private/server.key"

[database]
driver = "postgresql"
host = db.example.com
port = 5432
name = myapp_db
user = dbuser
password = "P@ssw0rd!123" ; secure password
pool_size = 20
timeout = 30

[logging]
level = "INFO"
file = "/var/log/myapp.log"
max_size = "100MB"
rotate = true

[cache]
enabled = true
type = "redis"
host = "cache.example.com:6379"
ttl = 3600

)ini";

	IniParser parser;
	bool ok = parser.parse(ini);

	ASSERT_TRUE(ok, "Complex INI should parse");
	ASSERT_EQ(parser.get("", "app_name").value_or(""), "MyApp", "App name");
	ASSERT_EQ(parser.get("", "version").value_or(""), "1.2.3", "Version");
	ASSERT_EQ(parser.get("server", "host").value_or(""), "0.0.0.0", "Server host");
	ASSERT_EQ(parser.get("server", "port").value_or(""), "8080", "Server port");
	ASSERT_EQ(parser.get("database", "password").value_or(""), "P@ssw0rd!123", "DB password");
	ASSERT_EQ(parser.get("logging", "level").value_or(""), "INFO", "Log level");
	ASSERT_EQ(parser.get("cache", "ttl").value_or(""), "3600", "Cache TTL");
}

void test_data_iteration() {
	TEST_CASE("Data Iteration");

	const char* ini = R"ini(
key1 = val1
[sec1]
key2 = val2
[sec2]
key3 = val3
)ini";

	IniParser parser;
	parser.parse(ini);

	int section_count = 0;
	int key_count = 0;

	for (const auto& sec_pair : parser.data()) {
		section_count++;
		for (const auto& kv : sec_pair.second) {
			key_count++;
		}
	}

	ASSERT_EQ(section_count, 3, "Should have 3 sections (including empty)");
	ASSERT_EQ(key_count, 3, "Should have 3 total keys");
}

int main() {
	std::cout << "========================================" << std::endl;
	std::cout << "   INI Parser Comprehensive Test Suite" << std::endl;
	std::cout << "========================================" << std::endl;

	test_basic_parsing();
	test_comments();
	test_sections();
	test_quoted_values();
	test_escape_sequences();
	test_whitespace_handling();
	test_empty_values();
	test_multiline_and_continuation();
	test_edge_cases();
	test_invalid_syntax();
	test_special_characters();
	test_section_ordering();
	test_complex_real_world();
	test_data_iteration();

	std::cout << "\n========================================" << std::endl;
	std::cout << "Test Results:" << std::endl;
	std::cout << "  Passed: " << tests_passed << std::endl;
	std::cout << "  Failed: " << tests_failed << std::endl;
	std::cout << "  Total:  " << (tests_passed + tests_failed) << std::endl;
	std::cout << "========================================" << std::endl;

	if (tests_failed == 0) {
		std::cout << "\n🎉 All tests passed!" << std::endl;
		return 0;
	} else {
		std::cout << "\n❌ Some tests failed!" << std::endl;
		return 1;
	}
}