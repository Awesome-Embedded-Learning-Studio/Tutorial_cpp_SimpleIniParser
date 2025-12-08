#include "ini_parse.h"
#include "string_splits.h"
#include "string_trim.h"
#include <optional>
#include <string>
#include <string_view>

namespace {
constexpr inline bool is_comments(const std::string_view sv) {
	const char is_comments_ch = sv.front();
	if (is_comments_ch == ';' || is_comments_ch == '#') {
		return true;
	}
	return false;
}

/**
 * @brief Check if the position inside quote
 *
 * @param line
 * @param pos
 * @return true
 * @return false
 */
constexpr inline bool isPositionInsideQuotes(std::string_view line, size_t pos) {
	bool in_double = false;
	bool in_single = false;
	for (size_t i = 0; i < pos && i < line.size(); ++i) {
		if (line[i] == '"' && !in_single) {
			bool escaped = (i > 0 && line[i - 1] == '\\');
			if (!escaped)
				in_double = !in_double;
		} else if (line[i] == '\'' && !in_double) {
			bool escaped = (i > 0 && line[i - 1] == '\\');
			if (!escaped)
				in_single = !in_single;
		}
	}
	return in_double || in_single;
}

std::string unquoteAndUnescape(const std::string_view& sv) {
	const size_t n = sv.size();
	if (n < 2)
		return std::string { sv };

	const char first = sv.front();
	const char last = sv.back();

	if (!((first == '"' && last == '"') || (first == '\'' && last == '\'')))
		return std::string { sv };

	std::string_view inner { sv.data() + 1, n - 2 };
	std::string out;
	out.reserve(inner.size());

	for (size_t i = 0; i < inner.size(); ++i) {
		char c = inner[i];

		if (c == '\\' && i + 1 < inner.size()) {
			char next = inner[i + 1];
			switch (next) {
			case 'n':
				out.push_back('\n');
				break;
			case 't':
				out.push_back('\t');
				break;
			case '\\':
				out.push_back('\\');
				break;
			case '"':
				out.push_back('"');
				break;
			case '\'':
				out.push_back('\'');
				break;
			default:
				out.push_back(next);
				break;
			}
			++i;
		} else {
			out.push_back(c);
		}
	}
	return out;
}

}

namespace cxx_utils::ini_parser {

bool IniParser::consume_line(const std::string_view& line,
                             std::string& current_section) {
	const auto preprocessed_line = cxx_utils::string::trim_view(line);
	if (preprocessed_line.empty()) {
		return true;
	}

	// leak out the preprocessed line
	if (is_comments(preprocessed_line)) {
		return true;
	}

	// check if is section
	if (preprocessed_line.front() == '[') {
		auto end_section_pos = preprocessed_line.find(']');
		if (end_section_pos == std::string::npos) {
			return false; // Not expected
		}

		auto section_sv = preprocessed_line.substr(1, end_section_pos - 1);
		section_sv = string::trim_view(section_sv);

		if (end_section_pos + 1 < preprocessed_line.size()) {
			for (size_t i = end_section_pos + 1; i < preprocessed_line.size(); ++i) {
				const char c = preprocessed_line[i];
				if ((c == ';' || c == '#') && !isPositionInsideQuotes(preprocessed_line, i)) {
					break;
				}
				if (!std::isspace(static_cast<unsigned char>(c))) {
					return false;
				}
			}
		}

		current_section = section_sv;
		if (m_data.find(current_section) == m_data.end()) {
			m_data.emplace(current_section, ini_section_t {});
		}
		return true; // Line finished
	}

	size_t comment_pos = std::string::npos;
	for (size_t i = 0; i < preprocessed_line.size(); ++i) {
		if ((preprocessed_line[i] == ';' || preprocessed_line[i] == '#')
		    && !isPositionInsideQuotes(preprocessed_line, i)) {
			comment_pos = i;
			break;
		}
	}

	const auto without_comment = (comment_pos == std::string::npos) ? preprocessed_line : preprocessed_line.substr(0, comment_pos);

	// Alright Split it
	const auto splited_kv = string::splits(
	    without_comment, "=");

	if (splited_kv.size() < 2) {
		return false; // Dont process
	}

	std::string key { string::trim_view(splited_kv[0]) };

	// value = join the rest with '='
	std::string value;
	{
		const auto pos = without_comment.find('=');
		if (pos == std::string::npos)
			return false;

		value = unquoteAndUnescape(string::trim_src(without_comment.substr(pos + 1)));
	}

	if (key.empty())
		return false;
	m_data[current_section][key] = value;

	return true;
}

bool IniParser::parse(const std::string_view view) {
	// reset everything
	clear();
	m_data.emplace("", ini_section_t {});
	auto lines = cxx_utils::string::splits(view, "\n");
	std::string current_section = "";
	for (const auto l : lines) {
		if (string::trim_view(l).empty())
			continue;

		consume_line(l, current_section);
	}

	return true;
}

std::optional<std::string>
IniParser::get(const std::string& section,
               const std::string& key) {
	auto sit = m_data.find(section);
	if (sit == m_data.end())
		return std::nullopt;
	auto kit = sit->second.find(key);
	if (kit == sit->second.end())
		return std::nullopt;
	return kit->second;
}

std::string
IniParser::get(const std::string& section,
               const std::string& key,
               const std::string& default_value) {
	auto sit = m_data.find(section);
	if (sit == m_data.end())
		return default_value;
	auto kit = sit->second.find(key);
	if (kit == sit->second.end())
		return default_value;
	return kit->second;
}

bool IniParser::has(const std::string& section,
                    const std::string& key) {
	auto sit = m_data.find(section);
	if (sit == m_data.end())
		return false;
	return sit->second.find(key) != sit->second.end();
}

}
