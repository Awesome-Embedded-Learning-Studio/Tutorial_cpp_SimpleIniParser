#include <iostream>
#include <string>
#include <string_view>

// std::string remove_one_prefix(const std::string &s) { return s.substr(1); }

// views -> string C++17

int main() {
  std::string_view sv = "hello";
  std::string owned_sv(sv);
  std::cout << owned_sv;
}