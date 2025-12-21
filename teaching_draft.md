使用下面的指令进行编译（没讲构建系统，凑合着用）

```bash
g++ -std=c++17 -O2 demo.cpp -o demo
./demo
```

---

## Demo 1 — 基本构造与打印

```cpp
#include <iostream>
#include <string>
#include <string_view>

int main() {
    std::string_view sv1 = "hello world";         // 从字面量
    std::string s = "hello string";
    std::string_view sv2(s);                       // 从 std::string
    const char buf[] = {'a','b','c','d'};          // 非以0结尾的缓冲区
    std::string_view sv3(buf, 4);                  // 指定长度的视图

    std::cout << sv1 << '\n' << sv2 << '\n' << sv3 << '\n';
}
```

---

## Demo 2 — 子视图、remove_prefix/remove_suffix（高效切片）

```cpp
#include <iostream>
#include <string_view>

int main() {
    std::string_view sv = "apple,banana,carrot";
    // 取前5个字符的视图（不拷贝）
    auto fruit = sv.substr(0, 5); // "apple"
    std::cout << fruit << '\n';

    // 模拟逐段解析：移除已处理部分
    sv.remove_prefix(6); // 去掉 "apple,"
    auto next = sv.substr(0, sv.find(',')); // "banana"
    std::cout << next << '\n';
}
```

---

## Demo 3 — 作为函数参数（统一接受 literal/string）

```cpp
#include <iostream>
#include <string_view>

void greet(std::string_view name) {
    std::cout << "Hi, " << name << "!\n";
}

int main() {
    greet("Alice");                // 字面量
    std::string bob = "Bob";
    greet(bob);                    // std::string
    const char* c = "Charlie";
    greet(c);                      // C 字符串
}
```

---

## Demo 4 — 查找/比较（替代 starts_with 的简单写法）

> 注意：`starts_with` 是 C++20，C++17 用 `compare` 或 `substr` 来判断

```cpp
#include <iostream>
#include <string_view>

bool starts_with(std::string_view sv, std::string_view prefix) {
    return sv.size() >= prefix.size() && sv.substr(0, prefix.size()) == prefix;
}

int main() {
    std::string_view v = "HTTP/1.1 200 OK";
    std::cout << std::boolalpha << starts_with(v, "HTTP") << '\n'; // true
}
```

---

## Demo 5 — 危险示例：悬空视图（必须强调！）

```cpp
#include <iostream>
#include <string_view>

int main() {
    // 坏写法：下面 sv 指向临时 string 的数据，临时对象销毁后 sv 悬空
    std::string_view sv = std::string("temp data"); // UB!
    // 访问 sv 是未定义行为——切记不要这样做
    std::cout << sv << '\n'; // 可能输出正确，也可能崩溃
}
```

演示时应故意运行并说明“这段代码是未定义行为，实际结果不可预测”。

---

## Demo 6 — 转换回 std::string（需要拥有字符串副本时）

```cpp
#include <iostream>
#include <string>
#include <string_view>

int main() {
    std::string_view sv = "some long content";
    // 如果需要保留或修改内容，必须拷贝为 std::string
    std::string owned(sv);
    owned += " (owned)";
    std::cout << owned << '\n';
}
```

---

`std::string_view` 本质上只是“指向字符串的轻量只读窗口”：它保存了一个指针和长度，所以能非常高效地表示子串而不做拷贝。用在函数参数时能让接口同时友好地接受字面量、`const char*` 和 `std::string`，写解析代码（比如逐段切分、trim、查找前缀）时 `remove_prefix` / `substr` 特别方便。但要格外小心生命周期：`string_view` 不拥有数据，指向的源如果是临时 `std::string` 或已经释放的缓冲区就会悬空并导致未定义行为；如果需要修改或长期保存，请马上拷贝成 `std::string`。总之，把 `string_view` 当作读视图使用能带来性能与接口整洁度的双重好处，但别把它当成能替代所有字符串所有权场景的“万能解”。
