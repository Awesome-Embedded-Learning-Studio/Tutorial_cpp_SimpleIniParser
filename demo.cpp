#include <charconv>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <unordered_map>

static const std::unordered_map<std::string_view, std::string> env_mappings = {
    {"OS", "Windows"},
    {"PATH", ""},
};
std::string get_env(std::string_view sv) {
  const auto it = env_mappings.find(sv);
  if (it != env_mappings.end()) {
    return it->second;
  } else {
    throw std::invalid_argument("No target " + std::string{sv} +
                                " value found!");
  }
}

std::optional<int> parse_int(std::string_view sv) {

  int value = 0;

  auto [ptr, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), value);

  if (ec == std::errc()) {
    return value;
  }

  return std::nullopt;
}

int main() {
  auto value = parse_int("1234");
  std::cout << value.value() << std::endl;

  value = parse_int("fcvnaiofhioqn");
  std::cout << value.value();

}