#include <string_view>

#include <iostream>
#include <string>
#include <string_view>

using namespace std;

void print_title(string_view title) {
    cout << "\n==============================\n";
    cout << title << '\n';
    cout << "==============================\n";
}

// Demo 1: 基本构造
void demo_basic_construct() {
    print_title("Demo 1: Basic Construction");

    std::string_view sv1 = "hello world";

    std::string s = "hello string";
    std::string_view sv2(s);

    const char buf[] = {'a', 'b', 'c', 'd'};
    std::string_view sv3(buf, 4);

    cout << "sv1: " << sv1 << '\n';
    cout << "sv2: " << sv2 << '\n';
    cout << "sv3: " << sv3 << '\n';
}

// Demo 2: 子视图与 remove_prefix
void demo_subview_and_prefix() {
    print_title("Demo 2: Substring & remove_prefix");

    std::string_view sv = "apple,banana,carrot";

    auto first = sv.substr(0, 5);
    cout << "first: " << first << '\n';

    sv.remove_prefix(6); // remove "apple,"
    auto second = sv.substr(0, sv.find(','));
    cout << "second: " << second << '\n';
}

// Demo 3: 作为函数参数
void greet(std::string_view name) {
    cout << "Hi, " << name << "!\n";
}

void demo_function_param() {
    print_title("Demo 3: Function Parameter");

    greet("Alice");

    std::string bob = "Bob";
    greet(bob);

    const char* c = "Charlie";
    greet(c);
}

// Demo 4: 查找与比较（C++17）
bool starts_with(std::string_view sv, std::string_view prefix) {
    return sv.size() >= prefix.size() &&
           sv.substr(0, prefix.size()) == prefix;
}

void demo_compare() {
    print_title("Demo 4: Compare & Starts With");

    std::string_view header = "HTTP/1.1 200 OK";

    cout << boolalpha;
    cout << "starts with HTTP: "
         << starts_with(header, "HTTP") << '\n';
}

// Demo 5: 危险示例（悬空 string_view）
void demo_dangling_view() {
    print_title("Demo 5: Dangling string_view (UB)");

    // 未定义行为示例：不要在真实代码中这样写
    std::string_view sv = std::string("temporary string");

    cout << "sv: " << sv << '\n';
    cout << "(This is undefined behavior!)\n";
}

// Demo 6: 转回 std::string
void demo_to_string() {
    print_title("Demo 6: Convert to std::string");

    std::string_view sv = "some long content";

    std::string owned(sv);
    owned += " (owned)";

    cout << owned << '\n';
}

int main() {
    demo_basic_construct();
    demo_subview_and_prefix();
    demo_function_param();
    demo_compare();
    demo_dangling_view();
    demo_to_string();

    print_title("End of string_view Demo");
    return 0;
}
