#include "ini_parser.h"
#include "string_splits.h"
#include "string_trim.h"
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>

namespace {

constexpr inline bool is_comments(const char ch) {
  return ch == ';' || ch == '#';
}

constexpr inline bool is_comments(const std::string_view sv) {
  const char first_ch = sv.front();
  return is_comments(first_ch);
}

std::string unquoteAndUnescape(const std::string &sv) {
  const size_t n = sv.size();
  if (n < 2)
    return std::string(sv);

  const char first = sv.front();
  const char last = sv.back();
  if (!((first == '"' && last == '"') || (first == '\'' && last == '\''))) {
    return std::string{sv};
  }

  std::string_view inner{sv.data() + 1, n - 2};
  std::string out;

  out.reserve(inner.size());

  for (size_t i = 0; i < inner.size(); i++) {
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
      // \n
      ++i;
    } else {
      out.push_back(c);
    }
  }

  return out;
}

// 在不在单引号或者是双引号里面
constexpr inline bool isPositionInsideQuotes(const std::string_view line,
                                             size_t pos) {
  bool in_double = false;
  bool in_single = false;
  for (size_t i = 0; i < pos && i < line.size(); i++) {
    if (line[i] == '"' && !in_single) {
      bool escape = (i > 0 && line[i - 1] == '\\'); //
      if (!escape) {
        in_double = !in_double;
      }
    } else if (line[i] == '\'' && !in_double) {
      bool escape = (i > 0 && line[i - 1] == '\\'); //
      if (!escape) {
        in_single = !in_single;
      }
    }
  }
  return in_double || in_single;
}
} // namespace

bool IniParser::parse(const std::string_view view) {
  using namespace cxx_utils::string;
  clear();
  // 不管有没有节名，最顶上的几个key value
  m_data.emplace("", ini_section_t{}); // 给最顶上的或者是空的节给予默认的键值对

  // \n
  auto lines = splits_v2_fix(view, '\n');
  std::string current_section = "";
  bool is_ok = true;
  for (const auto &line : lines) {
    auto trimmed = trim(line);
    if (trimmed.empty()) {
      continue;
    }

    if (is_comments(trimmed)) {
      continue;
    }

    // 处理每一行
    is_ok &= consume_line(trimmed, current_section);
    if (!is_ok)
      return false;
  }

  return true;
}

bool IniParser::consume_line(const std::string_view line,
                             std::string &section) {
  using namespace cxx_utils::string;

  // 处理节
  if (line.front() == '[') {
    auto end_pos = line.find(']');
    if (end_pos == std::string::npos) {
      return false;
    }

    auto section_name = line.substr(1, end_pos - 1);
    section = std::string{trim(section_name)};
    m_data.emplace(section, ini_section_t{});
    return true;
  }

  size_t comment_pos = std::string::npos;
  for (size_t i = 0; i < line.size(); ++i) {
    if (is_comments(line[i]) && !isPositionInsideQuotes(line, i)) {
      comment_pos = i;
      break;
    }
  }

  auto without_comment =
      (comment_pos == std::string::npos) ? line : line.substr(0, comment_pos);

  // 处理键值对 a=b
  auto splited = cxx_utils::string::splits_v2_fix(without_comment, '=');

  if (splited.size() < 3)
    return false;
  std::string key{trim(splited[0])};
  if (key.empty()) {
    return false;
  }

  std::string value_piece = "";
  for (int i = 2; i < splited.size(); i++) {
    value_piece += splited[i];
  }
  std::string value{trim(value_piece)};

  value = unquoteAndUnescape(value);
  m_data[section][key] = value;
  return true;
}

std::optional<std::string> IniParser::get(const std::string &section,
                                          const std::string &key) {
  // 拿指定的section去找ini_section_t
  const auto ini_section_it = m_data.find(section);
  if (ini_section_it == m_data.end()) {
    // 没找到
    return std::nullopt;
  }

  const auto value_it = ini_section_it->second.find(key);
  if (value_it == ini_section_it->second.end()) {
    // 没找到
    return std::nullopt;
  }

  return value_it->second;
}
