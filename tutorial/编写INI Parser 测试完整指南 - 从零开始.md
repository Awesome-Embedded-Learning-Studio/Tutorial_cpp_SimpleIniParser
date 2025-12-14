# INI Parser 测试编写完整指南 - 从零开始

## 前言

很多朋友基本上写完工程直接就跑了，的确，在之前我们编写了伪测试，对着他把我们的代码写完了，但是能不能过测试，**这个才是向其他人表示咱们的代码是靠谱的根本手段**

1. [测试的基本概念](#1-测试的基本概念)
2. [搭建测试框架](#2-搭建测试框架)
3. [编写第一个测试](#3-编写第一个测试)
4. [添加更多测试场景](#4-添加更多测试场景)
5. [完善测试套件](#5-完善测试套件)

------

## 测试的基本概念

先来点废话，看笔者这个教程的朋友大多应该没有什么工程概念，真的很有必要跟你说一说测试是什么。

测试，一个好理解的比喻就是给你的代码做"体检"。我们压根不关心里面你写的啥史山，不关心！关心的是你的代码行为产出是否具备通用性。所以，我们就像用户一样——搞定三个步骤：

- 喂给你的系统数据
- 然后执行
- 然后看对不对

简单！

------

## 搭建测试框架

其实我们大可以直接使用catch2啊等等一系列已经搞定的C/C++测试框架，但是笔者想了想，还是手搓一个简单的测试框架，先理解这里面的门道，之后使用catch2干活更利索。让我们从最简单的开始。创建一个新文件 `test_ini_parser.cpp`：

```cpp
#include "ini_parse.h"
#include <iostream>

int main() {
    std::cout << "开始测试..." << std::endl;
    return 0;
}
```

**现在编译运行一下，确保能正常工作！**

我们需要一些工具来帮助测试：

```cpp
#include "ini_parse.h"
#include <cassert>      // 用于断言
#include <iostream>     // 用于输出
#include <string>       // 用于字符串处理

using namespace cxx_utils::ini_parser;
```

> **💡 小贴士**：`using namespace` 让我们可以直接写 `IniParser` 而不用写 `cxx_utils::ini_parser::IniParser`，毕竟我们的测试还是专一的，这地方上梭哈完整的namespace多少没必要了

此外，我们需要知道通过了多少测试，失败了多少：

```cpp
// 在 main 函数之前添加
int tests_passed = 0;  // 通过的测试数
int tests_failed = 0;  // 失败的测试数
```

------

#### 创建测试宏 - TEST_CASE

这个宏让每个测试都有一个漂亮的标题：

```cpp
#define TEST_CASE(name) \
    std::cout << "\n=== " << name << " ===" << std::endl;
```

我们往里面嘎嘎写，就有了

```cpp
TEST_CASE("基本解析测试");
// 输出：=== 基本解析测试 ===
```

TEST_CASE更加实际的作用是将测试分组了，这里表示的就是一组话题相关的测试，比如说我们很快要搞的一组测试是否正确处理了空白字符等等的组测试。

------

#### 创建断言宏 - ASSERT_EQ

这是测试的核心！用来检查"实际值"是否等于"期望值"：

```cpp
#define ASSERT_EQ(actual, expected, msg) \
    do { \
        if ((actual) == (expected)) { \
            tests_passed++; \
            std::cout << "✓ " << msg << std::endl; \
        } else { \
            tests_failed++; \
            std::cout << "✗ " << msg << "\n  Expected: " << (expected) \
                     << "\n  Got: " << (actual) << std::endl; \
        } \
    } while (0)
```

**让我们拆解一下这个宏**：

1. **`do { ... } while(0)`**：这是个常见技巧，确保宏像一条语句一样工作
2. **条件判断**：比较 actual 和 expected
3. **成功时**：计数器+1，显示 ✓
4. **失败时**：计数器+1，显示 ✗ 和详细信息

```cpp
ASSERT_EQ(2 + 2, 4, "2加2应该等于4");
// 输出：✓ 2加2应该等于4
```

------

#### 添加 ASSERT_TRUE 和 ASSERT_FALSE

有时我们只需要检查条件是真还是假：

```cpp
#define ASSERT_TRUE(condition, msg) \
    do { \
        if (condition) { \
            tests_passed++; \
            std::cout << "✓ " << msg << std::endl; \
        } else { \
            tests_failed++; \
            std::cout << "✗ " << msg << " (条件失败)" << std::endl; \
        } \
    } while (0)

#define ASSERT_FALSE(condition, msg) \
    do { \
        if (!(condition)) { \
            tests_passed++; \
            std::cout << "✓ " << msg << std::endl; \
        } else { \
            tests_failed++; \
            std::cout << "✗ " << msg << " (期望为假)" << std::endl; \
        } \
    } while (0)
```

到这里对于我们的ini parser而言够了。

------

## 编写第一个最简单的测试 - test_basic_parsing

让我们从最基础的测试开始。先写一个空的函数框架：

```cpp
void test_basic_parsing() {
    // 这里将会是我们的测试代码
}
```

### 1.1 添加测试标题

首先，我们给这个测试一个好看的标题，这样运行时可以看到在测试什么：

```cpp
void test_basic_parsing() {
    TEST_CASE("Basic Parsing");
}
```

**运行后会显示**：

```
=== Basic Parsing ===
```

------

### 1.2 准备测试数据

现在我们需要一些 INI 数据来测试。我们使用 C++11 的原始字符串字面量：

```cpp
void test_basic_parsing() {
    TEST_CASE("Basic Parsing");
    
    const char* ini = R"ini(
key1 = value1
key2 = value2
)ini";
}
```

**解释**：

- `R"ini( ... )ini"` 是原始字符串
- 可以包含多行，不需要转义
- `ini` 是分隔符，你也可以用其他名字，比如 `R"***( ... )***"`
- **为什么用这个**：因为 INI 文件是多行的，用普通字符串会很麻烦

------

### 1.3 创建解析器并解析

接下来，我们创建一个解析器对象，并让它解析我们的数据：

```cpp
void test_basic_parsing() {
    TEST_CASE("Basic Parsing");
    
    const char* ini = R"ini(
key1 = value1
key2 = value2
)ini";
    
    IniParser parser;           // 创建解析器
    bool ok = parser.parse(ini); // 解析数据，返回是否成功
}
```

**现在的状态**：数据已经被解析了，但我们还没有验证结果

------

### 1.4 验证解析是否成功

我们的第一个断言：确保解析成功了

```cpp
void test_basic_parsing() {
    TEST_CASE("Basic Parsing");
    
    const char* ini = R"ini(
key1 = value1
key2 = value2
)ini";
    
    IniParser parser;
    bool ok = parser.parse(ini);
    
    ASSERT_TRUE(ok, "Parse should succeed");
}
```

**解释**：

- `ASSERT_TRUE` 检查 `ok` 是否为 true
- 第二个参数是描述信息
- 如果成功，会显示：`✓ Parse should succeed`
- 如果失败，会显示：`✗ Parse should succeed (condition failed)`

------

### 1.5 验证 key1 的值

现在我们要检查解析出来的值对不对。先检查 key1：

```cpp
void test_basic_parsing() {
    TEST_CASE("Basic Parsing");
    
    const char* ini = R"ini(
key1 = value1
key2 = value2
)ini";
    
    IniParser parser;
    bool ok = parser.parse(ini);
    
    ASSERT_TRUE(ok, "Parse should succeed");
    ASSERT_EQ(parser.get("", "key1").value_or(""), "value1", "Top-level key1");
}
```

**让我们拆解这一行**：

```cpp
parser.get("", "key1")    // 获取值，返回 std::optional<std::string>
      .value_or("")       // 如果没有值，返回空字符串
```

**参数说明**：

- 第一个 `""` 是 section 名，空字符串表示顶层（没有 section）
- 第二个 `"key1"` 是 key 名
- `.value_or("")` 的意思是：如果有值就返回值，没有就返回 `""`

**为什么要用 value_or**：

- `get()` 返回的是 `std::optional`，可能有值也可能没值
- 直接比较 optional 不方便
- 用 `value_or("")` 可以得到一个确定的字符串来比较

------

### 1.6 完成全部验证

现在我们再加上 key2 的验证：

```cpp
void test_basic_parsing() {
    TEST_CASE("Basic Parsing");
    
    const char* ini = R"ini(
key1 = value1
key2 = value2
)ini";
    
    IniParser parser;
    bool ok = parser.parse(ini);
    
    ASSERT_TRUE(ok, "Parse should succeed");
    ASSERT_EQ(parser.get("", "key1").value_or(""), "value1", "Top-level key1");
    ASSERT_EQ(parser.get("", "key2").value_or(""), "value2", "Top-level key2");
}
```

**完成**！这就是一个完整的基础测试了。

------

### 1.7 添加 Section 测试

现在我们让测试数据更复杂一点，加入一个 section：

```cpp
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
```

**注意变化**：

- INI 数据中加了 `[section1]` 和 `key3 = value3`
- 验证时第一个参数变成了 `"section1"`
- **这就是 section 的用法**

**恭喜！第一个测试完成了！** ✅

------

## 第二步：测试注释 - test_comments

现在我们来测试 INI 文件的注释功能。注释是很重要的，因为：

- 用户可能在配置文件里加注释
- 注释不应该影响解析结果

### 2.1 先写框架

```cpp
void test_comments() {
    TEST_CASE("Comment Handling");
}
```

------

### 2.2 准备带注释的数据

INI 文件支持两种注释：`;` 和 `#`

```cpp
void test_comments() {
    TEST_CASE("Comment Handling");
    
    const char* ini = R"ini(
; semicolon comment
# hash comment
key1 = value1
)ini";
}
```

**解释**：

- 第一行是分号注释（整行）
- 第二行是井号注释（整行）
- 第三行是实际的键值对

------

### 2.3 测试整行注释

```cpp
void test_comments() {
    TEST_CASE("Comment Handling");
    
    const char* ini = R"ini(
; semicolon comment
# hash comment
key1 = value1
)ini";
    
    IniParser parser;
    parser.parse(ini);
    
    ASSERT_EQ(parser.get("", "key1").value_or(""), "value1", "Value after comments");
}
```

**这个测试验证**：前面的注释不影响 key1 的解析

------

### 2.4 添加行内注释测试

注释也可以出现在行尾：

```cpp
void test_comments() {
    TEST_CASE("Comment Handling");
    
    const char* ini = R"ini(
; semicolon comment
# hash comment
key1 = value1 ; inline comment
key2 = value2 # inline hash comment
)ini";
    
    IniParser parser;
    parser.parse(ini);
    
    ASSERT_EQ(parser.get("", "key1").value_or(""), "value1", "Value before inline semicolon comment");
    ASSERT_EQ(parser.get("", "key2").value_or(""), "value2", "Value before inline hash comment");
}
```

**重点**：

- `value1` 后面的 `; inline comment` 应该被忽略
- 解析出来的值应该是 `value1`，不包含注释部分

------

### 2.5 测试引号保护注释符号

这是个容易出错的地方！如果值本身包含 `;` 或 `#`，需要用引号保护：

```cpp
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
```

**关键点**：

- `"value;with;semicolons"` 中的分号是值的一部分，不是注释
- 引号后面的 `; this is a comment` 才是注释
- 解析器应该正确区分这两种情况

**第二个测试完成！** ✅

------

## 第三步：测试 Section - test_sections

Section 是 INI 文件的重要功能，让配置可以分组。

### 3.1 基本框架

```cpp
void test_sections() {
    TEST_CASE("Section Handling");
}
```

------

### 3.2 准备测试数据

我们要测试几种情况：

1. 顶层键（在任何 section 之前）
2. 不同的 section
3. 同一个 section 出现两次

```cpp
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
}
```

**数据说明**：

- `toplevel = top` 在任何 section 之前
- `[section1]` 出现了两次
- 中间还有一个 `[section2]`

------

### 3.3 验证顶层键

```cpp
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
}
```

**验证**：section 为空字符串的键能正确读取

------

### 3.4 验证不同的 section

```cpp
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
    ASSERT_EQ(parser.get("section2", "key2").value_or(""), "val2", "Section2 key");
}
```

------

### 3.5 验证同一 section 多次出现

```cpp
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
```

**重要**：`[section1]` 出现两次，两次的键应该都能访问到

**第三个测试完成！** ✅

------

## 第四步：测试引号值 - test_quoted_values

引号让我们可以在值中包含特殊字符。

### 4.1 框架和基本引号

```cpp
void test_quoted_values() {
    TEST_CASE("Quoted Values");
    
    const char* ini = R"ini(
single = "quoted value"
)ini";
    
    IniParser parser;
    parser.parse(ini);
    
    ASSERT_EQ(parser.get("", "single").value_or(""), "quoted value", "Single quoted");
}
```

**测试**：基本的引号值

------

### 4.2 添加空格测试

引号的一个重要作用是保留空格：

```cpp
void test_quoted_values() {
    TEST_CASE("Quoted Values");
    
    const char* ini = R"ini(
single = "quoted value"
double = "value with spaces"
)ini";
    
    IniParser parser;
    parser.parse(ini);
    
    ASSERT_EQ(parser.get("", "single").value_or(""), "quoted value", "Single quoted");
    ASSERT_EQ(parser.get("", "double").value_or(""), "value with spaces", "Spaces in quotes");
}
```

------

### 4.3 测试空引号

```cpp
void test_quoted_values() {
    TEST_CASE("Quoted Values");
    
    const char* ini = R"ini(
single = "quoted value"
double = "value with spaces"
empty = ""
)ini";
    
    IniParser parser;
    parser.parse(ini);
    
    ASSERT_EQ(parser.get("", "single").value_or(""), "quoted value", "Single quoted");
    ASSERT_EQ(parser.get("", "double").value_or(""), "value with spaces", "Spaces in quotes");
    ASSERT_EQ(parser.get("", "empty").value_or(""), "", "Empty quotes");
}
```

**测试**：`""` 应该表示空字符串

------

### 4.4 引号中的等号

等号在 INI 中是特殊字符，但引号可以保护它：

```cpp
void test_quoted_values() {
    TEST_CASE("Quoted Values");
    
    const char* ini = R"ini(
single = "quoted value"
double = "value with spaces"
empty = ""
withequals = "key=value"
)ini";
    
    IniParser parser;
    parser.parse(ini);
    
    ASSERT_EQ(parser.get("", "single").value_or(""), "quoted value", "Single quoted");
    ASSERT_EQ(parser.get("", "double").value_or(""), "value with spaces", "Spaces in quotes");
    ASSERT_EQ(parser.get("", "empty").value_or(""), "", "Empty quotes");
    ASSERT_EQ(parser.get("", "withequals").value_or(""), "key=value", "Equals in quotes");
}
```

------

### 4.5 完整版本（加入密码示例）

```cpp
void test_quoted_values() {
    TEST_CASE("Quoted Values");
    
    const char* ini = R"ini(
single = "quoted value"
double = "value with spaces"
empty = ""
withequals = "key=value"
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
```

**测试**：密码中的 `;` 被引号保护，不会被当作注释

**第四个测试完成！** ✅

------

## 第五步：测试转义序列 - test_escape_sequences

转义序列让我们可以表示特殊字符。

### 5.1 换行符 `\n`

```cpp
void test_escape_sequences() {
    TEST_CASE("Escape Sequences");
    
    const char* ini = R"ini(
newline = "line1\nline2"
)ini";
    
    IniParser parser;
    parser.parse(ini);
    
    ASSERT_EQ(parser.get("", "newline").value_or(""), "line1\nline2", "Newline escape");
}
```

**重点**：

- INI 文件中是 `\n`（两个字符）
- 解析后应该变成真正的换行符（一个字符）

------

### 5.2 添加制表符 `\t`

```cpp
void test_escape_sequences() {
    TEST_CASE("Escape Sequences");
    
    const char* ini = R"ini(
newline = "line1\nline2"
tab = "col1\tcol2"
)ini";
    
    IniParser parser;
    parser.parse(ini);
    
    ASSERT_EQ(parser.get("", "newline").value_or(""), "line1\nline2", "Newline escape");
    ASSERT_EQ(parser.get("", "tab").value_or(""), "col1\tcol2", "Tab escape");
}
```

------

### 5.3 反斜杠和引号

```cpp
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
```

**转义规则**：

- `\\` → `\`（一个反斜杠）
- `\"` → `"`（一个引号）

**第五个测试完成！** ✅

------

## 继续下一组...

现在你已经掌握了基本模式！接下来的测试都遵循相同的步骤：

1. 写 `TEST_CASE` 标题
2. 准备测试数据（`R"ini( ... )ini"`）
3. 创建解析器并解析
4. 用 `ASSERT_EQ`、`ASSERT_TRUE` 等验证结果

让我快速过一下剩余的测试思路...

------

## 第六步：空白字符处理 - test_whitespace_handling

**测试目标**：解析器应该智能处理空格

```cpp
void test_whitespace_handling() {
    TEST_CASE("Whitespace Handling");
    
    const char* ini = R"ini(
key1=value1              // 没有空格
key2 = value2            // 正常空格
key3  =  value3          // 多个空格
  key4  =  value4        // 前后都有空格
)ini";
    
    // 解析后，所有值都应该是干净的，没有多余空格
}
```

**期望**：不管有多少空格，值都应该被正确提取

------

## 第七步：空值 - test_empty_values

**测试目标**：值可以是空的

```cpp
void test_empty_values() {
    TEST_CASE("Empty Values");
    
    const char* ini = R"ini(
empty1 =
empty2 = 
empty3 =   
quoted_empty = ""
)ini";
    
    // 所有这些都应该返回空字符串
}
```

**注意**：使用 `.value_or("NONE")` 来测试空值和不存在的区别

------

## 第八步：边界情况 - test_edge_cases

**测试目标**：处理各种极端情况

```cpp
void test_edge_cases() {
    TEST_CASE("Edge Cases");
    
    // 情况1：完全空的文件
    {
        IniParser parser;
        bool ok = parser.parse("");
        ASSERT_TRUE(ok, "Empty file should parse");
    }
    
    // 情况2：只有注释
    {
        const char* ini = "; comment\n# comment";
        IniParser parser;
        bool ok = parser.parse(ini);
        ASSERT_TRUE(ok, "Comments-only should parse");
    }
    
    // ... 更多边界情况
}
```

**用花括号 `{ }` 的原因**：创建独立的作用域，避免变量名冲突

------

## 第九步：复杂真实案例 - test_complex_real_world

**测试目标**：模拟真实配置文件

```cpp
void test_complex_real_world() {
    TEST_CASE("Complex Real-World Example");
    
    const char* ini = R"ini(
; Application Configuration
app_name = MyApp
version = 1.2.3

[server]
host = 0.0.0.0
port = 8080

[database]
driver = "postgresql"
password = "P@ssw0rd!123" ; secure password
)ini";
    
    // 验证多个 section，多种类型的值
}
```

**价值**：确保在复杂场景下也能正确工作

------

## Summary

每个测试都遵循这个模式：

```cpp
void test_xxx() {
    // 1. 标题
    TEST_CASE("描述");
    
    // 2. 准备数据
    const char* ini = R"ini(
    ... 测试数据 ...
    )ini";
    
    // 3. 解析
    IniParser parser;
    parser.parse(ini);
    
    // 4. 验证
    ASSERT_EQ(实际值, 期望值, "描述");
    ASSERT_TRUE(条件, "描述");
    ASSERT_FALSE(条件, "描述");
}
```

**关键技巧**：

1. 用原始字符串 `R"ini( ... )ini"` 写测试数据
2. 用 `.value_or("")` 处理 optional
3. 每个断言都写清楚描述
4. 从简单到复杂，逐步添加测试

### 完整的 main 函数

把所有测试组织起来：

```cpp
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
```

