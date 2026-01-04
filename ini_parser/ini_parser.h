#pragma once
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

class IniParser {
  // 节下的键值对
  using ini_section_t = std::unordered_map<std::string, std::string>;
  // 节对应若干个键值对
  std::unordered_map<std::string, // section name
                     ini_section_t>
      m_data;
  void clear() { m_data.clear(); }
  bool consume_line(const std::string_view line, std::string &section);

public:
  bool parse(const std::string_view view);
  std::optional<std::string> get(const std::string &section,
                                 const std::string &key); // -> value
};