使用下面的指令进行编译（没讲构建系统，凑合着用）

```bash
g++ -std=c++17 -O2 demo.cpp -o demo
./demo
```

## 1. 为什么我们需要 `std::optional`

在 C++ 中，我们经常遇到这样的问题：

> 一个函数**可能成功，也可能失败**
> 但失败并不一定是“异常”

例如：

* 字符串解析为整数
* 查找配置项
* 查 map / cache
* 读取一个“可能不存在”的资源

### 传统做法的问题

```cpp
int parse_int(const std::string& s); // 失败返回 -1？
```

* ❌ -1 是魔法值
* ❌ 无法区分 “解析失败” 和 “值就是 -1”
* ❌ 接口语义靠注释，而不是类型

**这正是 `std::optional` 的切入点。**

---

## 2. `std::optional` 的核心语义

```cpp
std::optional<T>
```

它表达的不是“错误”，而是：

> **“这里可能有一个 T，也可能没有”**

这是一种 **接口契约（Contract）**，而不是实现细节。

---

## 3. Demo 1：解析字符串为整数（核心示例）

这是最适合演示 `std::optional` 的例子之一。

### 3.1 函数实现

```cpp
#include <optional>
#include <string>
#include <charconv>

std::optional<int> parse_int(const std::string& s)
{
    int value = 0;

    auto [ptr, ec] =
        std::from_chars(s.data(), s.data() + s.size(), value);

    if (ec == std::errc())
        return value;

    return std::nullopt;
}
```

### 3.2 讲解要点

* 不抛异常
* 不返回魔法值
* 用返回类型明确告诉调用者：

  > **“这个函数可能没有结果”**

---

### 3.3 使用方式（推荐写法）

```cpp
void demo()
{
    if (auto v = parse_int("123")) {
        std::cout << "value = " << *v << '\n';
    } else {
        std::cout << "parse failed\n";
    }
}
```

### 为什么这个写法很好？

* `if (auto v = ...)` 是 **C++17 以后最自然的 optional 用法**
* `*v` 的语义非常直观：

  > 我已经检查过你有值了

---

## 4. `optional` 不是异常，也不是错误码

这里很重要的一点是：
**`std::optional` ≠ 错误处理银弹**

### 它擅长的事情

* 查找成功 / 失败
* 解析成功 / 失败
* 可选配置项
* cache 命中 / 未命中

### 它不擅长的事情

* 携带错误原因
* 区分多种失败类型
* 复杂的恢复逻辑

---

## 5. Demo 2：查找配置项（工程化示例）

```cpp
#include <optional>
#include <unordered_map>
#include <string>

std::optional<std::string>
get_env(const std::unordered_map<std::string, std::string>& env,
        const std::string& key)
{
    auto it = env.find(key);
    if (it == env.end())
        return std::nullopt;
    return it->second;
}
```

### 使用：

```cpp
if (auto path = get_env(env, "PATH")) {
    std::cout << "PATH=" << *path << '\n';
} else {
    std::cout << "PATH not found\n";
}
```

### 讲解点

* 比返回空字符串更清晰
* 比返回指针更安全
* 比异常更轻量

---

## 6. `value()` 是陷阱，`value_or()` 是朋友

### ⚠️ 不推荐的写法

```cpp
auto v = parse_int("abc");
int x = v.value(); // 可能抛 std::bad_optional_access
```

> `optional` 的设计初衷不是让你“赌它有值”。

---

### ✅ 推荐用法：`value_or`

```cpp
int timeout = get_env(env, "TIMEOUT")
                  .value_or("30")
                  .size();
```

或更直观一点：

```cpp
int timeout = 30;
if (auto t = get_env(env, "TIMEOUT")) {
    timeout = std::stoi(*t);
}
```

**这正是 optional 的爽点：**

> 有就用，没有就优雅地给默认值

---

## 7. `std::optional` vs 其他错误处理方式

| 方式                       | 适合场景       |
| ------------------------ | ---------- |
| `std::optional`          | 可预期的失败、缺失值 |
| 异常                       | 真正的异常路径    |
| 错误码 / `error_code`       | 需要错误原因     |
| `expected / Result<T,E>` | 复杂错误语义     |

一句话总结：

> **optional 解决的是“有没有”，不是“为什么没有”**

---

## 8. 最佳实践总结

* 用 `std::optional` 表达“可能无值”的返回结果
* 不要滥用 `value()`
* API 文档中说明 `nullopt` 的语义
* 需要错误原因时，考虑 `expected / Result`

---

## 9. 结语

`std::optional` 的价值不在于“新”，
而在于它把一个长期依赖注释和约定的问题：

> **“这个函数到底会不会失败？”**

交还给了 **类型系统本身**。

