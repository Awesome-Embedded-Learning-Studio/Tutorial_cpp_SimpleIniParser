# 现代C++工程实践：简单的IniParser4——实现ini_parser

## 前言

在上一篇博客中，我们已经完成了split的优化。现在我们即将开始我们工作的核心。这就是说，在项目工程的前期，我们把一些基建搞定了，由于IniParser很简单，所以在这个工程中我们的基建基本上算是做完了（当然其他的类似IO流的封装我们不做处理，我们的核心还是IniParser）

我们重新回顾一下需求：

> 我们要编写一个解析器，逐行的解析我们的状态——确定当前行所在的section，以及对应的Key value是如何的，对于双引号/单引号引的内容要转义。

我们重新设计一份接口：

```c++
/**
 * @file ini_parse.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief This is an on-for ini ASCII Parser
 * @version 0.1
 * @date 2025-12-06
 *
 * @copyright Copyright (c) 2025
 *
 */

#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

namespace cxx_utils {
namespace ini_parser {

	/**
	 * @brief   Ini parser accept parse a
	 *          simple ini file, which contains
	 *          only plain kv pairs
	 *
	 */
	class IniParser {
	public:
		IniParser() = default;

		IniParser(const IniParser&) = delete;
		const IniParser& operator=(const IniParser&) = delete;

		/**
		 * @brief parse a view-ini string
		 *
		 * @param view
		 * @return true
		 * @return false
		 */
		bool parse(const std::string_view view);

		/**
		 * @brief get the value from parser map
		 *
		 * @param section
		 * @param key
		 * @return std::optional<std::string>
		 */
		std::optional<std::string>
		get(const std::string& section,
		    const std::string& key);

		/**
		 * @brief get the value from ini file
		 *
		 * @param section
		 * @param key
		 * @param default_value
		 * @return std::string
		 */
		std::string
		get(const std::string& section,
		    const std::string& key,
		    const std::string& default_value);

		/**
		 * @brief   check if the mappings owns value,
		 *          briefly recommend when need to know if owns value
		 *
		 * @param section
		 * @param key
		 * @return true
		 * @return false
		 */
		bool
		has(const std::string& section,
		    const std::string& key);

		using ini_section_t = std::unordered_map<std::string, std::string>;
		using ini_data_t = std::unordered_map<std::string, ini_section_t>;

		/**
		 * @brief get the datas directly
		 *
		 * @return const ini_data_t
		 */
		inline const ini_data_t data() const noexcept { return m_data; }

		/**
		 * @brief clear the datas, reset the ini parser
		 *
		 */
		void clear() noexcept { return m_data.clear(); }

	private:
		ini_data_t m_data; ///< data_stores

	private:
		bool consume_line(const std::string_view& line,
		                  std::string& current_section);
	};

}
}
```

我们使用`bool parse(const std::string_view view);`来解析给定的字符串，为什么是`std::string_view`。我们要解析的 INI 文件长这样：

```ini
; 这是一个注释
[Server]
ip = 127.0.0.1
port = 8080

[User]
name = "John Doe"  ; 这是行内注释
message = "Hello\nWorld" ; 支持转义字符
```

回顾我们的任务，我们要支持的就是：

1. **Section（段落）**：比如 `[Server]`。
2. **Key-Value（键值对）**：比如 `ip = 127.0.0.1`。
3. **注释**：支持 `;` 和 `#`。
4. **复杂情况**：支持引号 `"` 包裹的值，以及引号内的转义字符（如 `\n`）。

# 如何行动起来

笔者编写的时候就苦恼，如果直接说给一个方案，那这个跟其他教程有啥区别啊。没意思，所以笔者决定倒过来，像通关一样说明我们的iniparser的基本要求做到什么样子，就像实际开发的时候用户需求反馈那样。

## 定义测试用例

我们来看看，咱们的处理器要达到一起什么样的要求

#### 测试用例1: 空行处理

```cpp
TEST_CASE("Empty lines should be ignored") {
    IniParser parser;
    std::string section;
    REQUIRE(parser.consume_line("", section) == true);
    REQUIRE(parser.consume_line("   ", section) == true);
    REQUIRE(parser.consume_line("\t\t", section) == true);
}
```

**需求解读**：空行和只包含空白字符的行应该被忽略，不影响解析结果。

#### 测试用例2: 注释行处理

```cpp
TEST_CASE("Comment lines should be ignored") {
    IniParser parser;
    std::string section;
    REQUIRE(parser.consume_line("; this is comment", section) == true);
    REQUIRE(parser.consume_line("# this is comment", section) == true);
    REQUIRE(parser.consume_line("  ; comment with spaces", section) == true);
}
```

**需求解读**：以`;`或`#`开头的行是注释，应该被完全忽略。注释符前面可以有空白。

#### 测试用例3: Section解析

```cpp
TEST_CASE("Section parsing") {
    IniParser parser;
    std::string section;
    REQUIRE(parser.consume_line("[database]", section) == true);
    REQUIRE(section == "database");
    
    REQUIRE(parser.consume_line("[  server  ]", section) == true);
    REQUIRE(section == "server");
}
```

**需求解读**：方括号内的内容是section名，需要trim空白。

#### 测试用例4: Section格式错误检测

```cpp
TEST_CASE("Invalid section format") {
    IniParser parser;
    std::string section;
    REQUIRE(parser.consume_line("[no_closing", section) == false);
    REQUIRE(parser.consume_line("[section]garbage", section) == false);
}
```

**需求解读**：格式错误的section应该返回false，包括：
- 没有闭合的`]`
- `]`后面有非空白、非注释字符

#### 测试用例5: 键值对解析

```cpp
TEST_CASE("Key-value parsing") {
    IniParser parser;
    std::string section = "test";
    parser.m_data[section] = {};
    
    REQUIRE(parser.consume_line("key=value", section) == true);
    REQUIRE(parser.m_data[section]["key"] == "value");
    
    REQUIRE(parser.consume_line("port = 8080", section) == true);
    REQUIRE(parser.m_data[section]["port"] == "8080");
}
```

**需求解读**：`key=value`格式，等号两边可以有空白，需要trim。

#### 测试用例6: 带引号的值

```cpp
TEST_CASE("Quoted values") {
    IniParser parser;
    std::string section = "test";
    parser.m_data[section] = {};
    
    REQUIRE(parser.consume_line("path=\"C:\\Users\\test\"", section) == true);
    REQUIRE(parser.m_data[section]["path"] == "C:\\Users\\test");
}
```

**需求解读**：值可以用引号包围，引号应该被移除，转义字符应该被处理。

#### 测试用例7: 行内注释

```cpp
TEST_CASE("Inline comments") {
    IniParser parser;
    std::string section = "test";
    parser.m_data[section] = {};
    
    REQUIRE(parser.consume_line("key=value ; comment", section) == true);
    REQUIRE(parser.m_data[section]["key"] == "value");
    
    // 引号内的;不是注释
    REQUIRE(parser.consume_line("msg=\"a;b\" ; real comment", section) == true);
    REQUIRE(parser.m_data[section]["msg"] == "a;b");
}
```

**需求解读**：`;`和`#`可以出现在行中间作为注释，但引号内的不算。

#### 测试用例8: 无效的键值对

```cpp
TEST_CASE("Invalid key-value pairs") {
    IniParser parser;
    std::string section = "test";
    
    REQUIRE(parser.consume_line("no_equals_sign", section) == false);
    REQUIRE(parser.consume_line("=no_key", section) == false);
}
```

**需求解读**：没有等号或key为空的行是无效的。

---

#### 测试用例1: 空行处理

空行的处理要使用专门的trim函数搞定，毕竟有时候咱们的输入不见得一定是严格格式化的，所以需要做一点预处理：

```cpp
std::string_view
	trim_view(
	    const std::string_view src_view,
	    TrimPolicy policy) {
		// if the src_view empty
		if (src_view.empty()) {
			return src_view;
		}

		size_t end = src_view.size();
		size_t start = 0;

		if (policy == TrimPolicy::Both || policy == TrimPolicy::Left) {
			while (start < end && is_space_char(src_view[start])) {
				start++;
			}
		}

		if (policy == TrimPolicy::Both || policy == TrimPolicy::Right) {
			while (end > start && is_space_char(src_view[end - 1])) {
				end--;
			}
		}

		return src_view.substr(start, end - start);
	}
```

首先，我们需要一个函数来去除字符串首尾的空白。这是处理空行的前提。上面就是一个example。

### 📝 第一版实现

```cpp
// 版本1: 最简单实现
bool IniParser::consume_line(const std::string_view& line,
                             std::string& current_section) {
    // 先去除首尾空白
    const auto preprocessed_line = cxx_utils::string::trim_view(line);
    
    // 测试1: 空行处理
    if (preprocessed_line.empty()) {
        return true;
    }
    
    // 测试2: 注释行处理
    if (preprocessed_line.front() == ';' || preprocessed_line.front() == '#') {
        return true;
    }
    
    return false; // 其他情况暂不处理
}
```

很快会有朋友注意到，当我们看到代码中有这样的判断：

```cpp
if (preprocessed_line.front() == ';' || preprocessed_line.front() == '#')
```

这段逻辑显然可能会在多个地方使用（比如行内注释检测），所以我们提取一个辅助函数：

```cpp
namespace {
    constexpr inline bool is_comments(const std::string_view sv) {
        const char is_comments_ch = sv.front();
        if (is_comments_ch == ';' || is_comments_ch == '#') {
            return true;
        }
        return false;
    }
}
```

> 这里笔者觉得很有必要聊下：
>
> - constexpr是现代C++的关键字，指导我们的编译器在编译期计算，这个有趣的关键字会专门开博客说明
> - `inline`：避免多次定义，注意，一些老教程会告诉你内联展开，现在inline没有这个意思了，注意，是没有这个意思了，他的真正含义是避免多次定义
> - namespace {}，这个呢，是匿名空间，过去很多朋友使用static来限制，一个更加现代和安全的做法是采用这个，这个时候编译器会随机生成保证并不重复的空间名称修饰这下面包裹的符号。

现在我们的代码干净了一些：

```cpp
bool IniParser::consume_line(const std::string_view& line,
                             std::string& current_section) {
    const auto preprocessed_line = cxx_utils::string::trim_view(line);
    if (preprocessed_line.empty()) {
        return true;
    }
    
    if (is_comments(preprocessed_line)) {
        return true;
    }
    
    return false;
}
```

---

#### 测试用例3 + 4: Section的处理流

下面我们要搞的就是Section的处理了。如果说前面还在小试身手，那这里显然就是一个小挑战了。**之后我们入手需求，先不要慌，观察被建模对象才是王道**，我们知道，在INI 文件中，Section 的典型形式是：

```ini
[network]
```

所以，我们可以先给自己定下**第一个最简单、也是最稳妥的规则**：

> ✅ **只要一行是以 `[` 开头的，就“有可能”是一个 Section 行**

注意这里的措辞是“有可能”，因为：

```ini
[network
```

这种明显是不合法的，但**第一步我们只负责识别候选行，而不是立刻判合法性**。所以熟悉STL的你，一下子就能写出代码，这一步通常只是：

```cpp
if (!line.empty() && line.front() == '[') {
    // 可能是 section
}
```

现在我们在if模块内，可以肯定的说“这是一个 Section 候选行”之后，下一步就是思考咋把section搞出来呢？合法的 Section 一定是：

```ini
[ section_name ]
```

因此我们的解析思路非常清晰：

1. 找到 **第一个 `]`**
2. 提取 `[` 和 `]` 中间的内容
3. 对结果做 `trim`（去掉首尾空白）

举个例子：

```ini
[   database   ]
```

最终得到的 section 名应当是：

```text
database
```

其实我们也顺手把非法的Section搞出来了——如果整行连 `]` 都找不到，那它一定是非法的 Section 行。

```ini
[database
```

这种情况应当直接返回 `false`，交由上层逻辑处理错误。

可是别急，我们还要思考，来看下面几种写法：

```ini
[core]        ; 合法，后面是注释
[core]        # 合法，后面是注释
```

但下面这种就不行了：

```ini
[core] abc
```

现在我们可以理清楚逻辑了，笔者列在下面：

- 找到 **第一个 `]`**后，利用STL String的substr拉取内容
- 找不到，说明是非法内容
- 我们还要检查 `]` 后面 **只允许出现空白字符或注释**，一旦出现其他内容，说明这一行是非法的 Section

### 📝 第二版实现

```cpp
bool IniParser::consume_line(const std::string_view& line,
                             std::string& current_section) {
    const auto preprocessed_line = cxx_utils::string::trim_view(line);
    if (preprocessed_line.empty()) {
        return true;
    }
    
    if (is_comments(preprocessed_line)) {
        return true;
    }
    
    // 新增：Section解析
    if (preprocessed_line.front() == '[') {
        // 查找闭合的 ]
        auto end_section_pos = preprocessed_line.find(']');
        if (end_section_pos == std::string::npos) {
            return false; // 测试4: 没有闭合括号
        }
        
        // 提取section名称并trim
        auto section_sv = preprocessed_line.substr(1, end_section_pos - 1);
        section_sv = string::trim_view(section_sv);
        
        // 测试4: 检查]后面是否有非空白字符（除了注释）
        if (end_section_pos + 1 < preprocessed_line.size()) {
            for (size_t i = end_section_pos + 1; i < preprocessed_line.size(); ++i) {
                const char c = preprocessed_line[i];
                // 如果遇到注释符号，剩余部分可以忽略
                if (c == ';' || c == '#') {
                    break;
                }
                // 如果是非空白字符，格式错误
                if (!std::isspace(static_cast<unsigned char>(c))) {
                    return false;
                }
            }
        }
        
        // 更新当前section
        current_section = section_sv;
        // 确保m_data中有这个section
        if (m_data.find(current_section) == m_data.end()) {
            m_data.emplace(current_section, ini_section_t{});
        }
        return true;
    }
    
    return false; // 其他情况暂不处理
}
```

但是实际上事情还没完，在检查`]`后面的内容时，我们简单地判断`c == ';' || c == '#'`就认为是注释。但如果写成这样呢？

```ini
[section] "quoted;text" ; actual comment
```

这种情况下，引号内的`;`不应该被当作注释开始。虽然这种写法很罕见，但为了健壮性，我们应该处理。

> 但是很紧急吗？比起来，一个项目应该首先可用，然后是逐步的完善健壮性，而不是在最开始就给自己埋复杂度，让自己的代码没有周旋的余地，所以，我们评估这个需求完全可以放到之后再迭代。

#### 做到键值对分割和存储

在 Section 能被正确识别之后，解析器接下来要面对的，就是 INI 文件中**最常见、也最核心的结构**：

```ini
key = value
```

在 INI 文件中，只要一行满足如下条件，我们想一想，这大概率就是一个键值对：不是空行，不是注释，更不是 Section（不以 `[` 开头）

但是最重要的显然是：百分百存在一个`=`，因此，最朴素的第一条规则是：通过 `=` 来分割 Key 和 Value。但真实世界从来不会这么“乖”。很有可能我们能拿到类似这样的东西：

```ini
url = https://example.com?a=1&b=2
token = abc==def==
```

仔细想想，这不就是ini规则中的—— **只按“第一个 `=`”分割**，Key取这个等号的左边，其他的是右边。换句话说：

```ini
a=b=c=d
```

应当被解析为：

```text
key   -> a
value -> b=c=d
```

------

### 📝 第三版实现

```cpp
bool IniParser::consume_line(const std::string_view& line,
                             std::string& current_section) {
    const auto preprocessed_line = cxx_utils::string::trim_view(line);
    if (preprocessed_line.empty()) {
        return true;
    }
    
    if (is_comments(preprocessed_line)) {
        return true;
    }
    
    if (preprocessed_line.front() == '[') {
        // ... section处理代码 (同迭代2)
        return true;
    }
    
    // 新增：键值对解析
    // 先简单按'='分割
    const auto splited_kv = string::splits(preprocessed_line, "=");
    
    if (splited_kv.size() < 2) {
        return false; // 测试8: 没有等号
    }
    
    std::string key{string::trim_view(splited_kv[0])};
    
    // value可能包含'='，所以重新提取
    const auto pos = preprocessed_line.find('='); // 我们当然可以自信的不下判断了，前面挡回去了不存在=的情况
    
    std::string value{string::trim_view(preprocessed_line.substr(pos + 1))};
    
    if (key.empty()) {
        return false; // 测试8: 空key
    }
    
    m_data[current_section][key] = value;
    return true;
}
```

#### 现在，我们的测试器基本可以正常工作了

但是我们的老问题没解决，就是引用的问题：“”所以我们就需要思考下面这个问题了：如何处理类似

```ini
msg="a;b" ; real comment
```

的ini呢？这里面有 **两个 `;`**，但它们的语义完全不同，这是因为——第一个 `;`在在 **双引号内部**，由此，他是字符串值的一部分，第二个不是，他在之外，必然是一个注释的起始符号，如果我们之前不去处理好他们，我们就会把`"a`当值了，后面拉一连串的全丢掉了。

所以，我们真的需要一个函数，判断我们是不是在一个引号内。

> 很多人会重复我们之前的想法——我能不能往前找最近的 `"`，再往后找一个 `"`？说得好，你很是一个软件复用的天才，但是很遗憾，你没考虑嵌套；单引号 vs 双引号；和转义字符 `\"`等问题，这也就意味着我们需要时刻判断是不是在一个引号内。

最小、也是最清晰的状态模型是：

- `in_double`：当前是否在双引号 `"` 内
- `in_single`：当前是否在单引号 `'` 内

并且遵守一个重要约定： **同一时刻，只可能在一种引号内**，单引号里的 `"` 只是普通字符，双引号里的 `'` 也是普通字符。

很自然的想法就是搞一个状态机。存住我们的状态——

- `bool in_double = false;`：记录我们是否处于**双引号** (`"`) 字符串内。
- `bool in_single = false;`：记录我们是否处于**单引号** (`'`) 字符串内。

然后，我们对视图从指定的位置开始扫描：

```cpp
namespace {
    constexpr inline bool isPositionInsideQuotes(std::string_view line, 
                                                  size_t pos) {
        bool in_double = false;
        bool in_single = false;

        for (size_t i = 0; i < pos && i < line.size(); ++i) {
			...
        }

        return in_double || in_single;
    }
}
```

当我们扫描到引号字符时（`'` 或 `"`），我们不能直接改变状态。我们需要先检查一个非常重要的条件：**它是否被转义了？**

```c++
bool escaped = (i > 0 && line[i - 1] == '\\');
```

- **转义的引号：** 如果引号的前一个字符是反斜杠（`\`），那么这个引号就是**被转义的**（例如：`\"`）。
- **处理：** 被转义的引号只是字符串中的一个普通字符，**它没有能力**开启或关闭一个字符串。
- **总结：** 只有**未被转义的引号**，才有资格改变我们的 `in_double` 或 `in_single` 状态。

```c++
namespace {
    constexpr inline bool isPositionInsideQuotes(std::string_view line, 
                                                  size_t pos) {
        bool in_double = false;
        bool in_single = false;

        for (size_t i = 0; i < pos && i < line.size(); ++i) {
            if (line[i] == '"' && !in_single) {
                // 检查是否被转义
                bool escaped = (i > 0 && line[i - 1] == '\\');
                if (!escaped)
                    in_double = !in_double;
            } 
            else if (line[i] == '\'' && !in_double) {
                bool escaped = (i > 0 && line[i - 1] == '\\');
                if (!escaped)
                    in_single = !in_single;
            }
        }

        return in_double || in_single;
    }
}
```

在改变状态时，代码遵循严格的互斥原则：

- **遇到双引号 (`"`):** 只有当我们**不在单引号内**（`!in_single`）时，这个双引号才有效。
- **遇到单引号 (`'`):** 只有当我们**不在双引号内**（`!in_double`）时，这个单引号才有效。

这个设计保证了字符串之间的嵌套不会互相干扰，例如在 `"Hello 'world'"` 这个双引号字符串中，里面的单引号不会被错误地当作字符串的起始或结束。

### 📝 第四版实现

```cpp
bool IniParser::consume_line(const std::string_view& line,
                             std::string& current_section) {
    const auto preprocessed_line = cxx_utils::string::trim_view(line);
    if (preprocessed_line.empty()) {
        return true;
    }
    
    if (is_comments(preprocessed_line)) {
        return true;
    }
    
    if (preprocessed_line.front() == '[') {
        auto end_section_pos = preprocessed_line.find(']');
        if (end_section_pos == std::string::npos) {
            return false;
        }
        
        auto section_sv = preprocessed_line.substr(1, end_section_pos - 1);
        section_sv = string::trim_view(section_sv);
        
        // 更新：使用isPositionInsideQuotes检查
        if (end_section_pos + 1 < preprocessed_line.size()) {
            for (size_t i = end_section_pos + 1; i < preprocessed_line.size(); ++i) {
                const char c = preprocessed_line[i];
                if ((c == ';' || c == '#') && 
                    !isPositionInsideQuotes(preprocessed_line, i)) {
                    break;
                }
                if (!std::isspace(static_cast<unsigned char>(c))) {
                    return false;
                }
            }
        }
        
        current_section = section_sv;
        if (m_data.find(current_section) == m_data.end()) {
            m_data.emplace(current_section, ini_section_t{});
        }
        return true;
    }
    
    // 新增：找到真正的注释位置（不在引号内）
    size_t comment_pos = std::string::npos;
    for (size_t i = 0; i < preprocessed_line.size(); ++i) {
        if ((preprocessed_line[i] == ';' || preprocessed_line[i] == '#')
            && !isPositionInsideQuotes(preprocessed_line, i)) {
            comment_pos = i;
            break;
        }
    }
    
    // 移除注释部分
    const auto without_comment = (comment_pos == std::string::npos) 
        ? preprocessed_line 
        : preprocessed_line.substr(0, comment_pos);
    
    // 分割键值对
    const auto splited_kv = string::splits(without_comment, "=");
    if (splited_kv.size() < 2) {
        return false;
    }
    
    std::string key{string::trim_view(splited_kv[0])};
    
    const auto pos = without_comment.find('=');
    if (pos == std::string::npos) {
        return false;
    }
    
    std::string value{string::trim_view(without_comment.substr(pos + 1))};
    
    if (key.empty()) {
        return false;
    }
    
    m_data[current_section][key] = value;
    return true;
}
```

#### 处理value中的字符串

在前面的步骤中，我们已经能够正确识别 `key = value` 的基本结构。但一个**合格的 INI 解析器**还必须处理一个常见且容易被忽略的细节：**值可能被引号包围，并且内部可能包含转义字符**。

```ini
path = "C:\\Program Files\\MyApp"
title = "Hello \"World\""
```

如果我们直接把等号右侧的字符串原样存下来，最终得到的值显然是不正确的。因此，在解析流程的最后阶段，需要对 value 再做一次“语义层面的清洗”。为此，我们引入第四个辅助函数：

```cpp
std::string unquoteAndUnescape(std::string_view raw);
```

它的职责非常单一，也非常明确：**移除合法的外层引号，并处理内部的转义序列**。

当然，并不是所有的值都需要处理引号，例如：

```ini
count = 42
enabled = true
```

因此，函数的第一件事并不是“无脑去引号”，而是**检查首尾字符是否构成一对匹配的引号**，只有在满足这个条件时，才认为这是一个被引号包围的字符串值。一旦确认成立，真正的内容位于：

```text
raw[1 ... raw.size() - 2]
```

否则，说明这是一个普通值，可以直接进入下一阶段（或原样返回）。

> 这个判断非常关键，它避免了如下错误行为：

```ini
path = C:\test\file   ; 不应该被当成引号字符串
```

之后就是我们处理转义了，去掉外层引号后，字符串内部仍然可能包含转义序列，例如：

- `\\` → `\`
- `\"` → `"`
- `\n` → 换行（是否支持取决于设计）

处理转义字符最稳妥的方式是：**手动遍历字符串**。

在处理开始时，我们初始化一个空的**结果字符串**（`result`），然后从输入字符串的第一个字符开始向右扫描。如果当前扫描到的字符**不是**反斜杠（`\`），那么该字符被视为普通数据，会直接追加到结果字符串的末尾，并继续扫描下一个字符。然而，如果当前字符是反斜杠（`\`），程序会进入**转义状态**，必须查看紧随其后的**下一个字符**。

如果这个序列（`\` 加上下一个字符，例如 `\n` 或 `\\`）是一个合法的转义约定，程序会将其作为一个整体进行转换，并将转换后的**单个字符**追加到结果字符串中，同时扫描指针会**推进两个位置**（消耗掉 `\` 和被转义的字符）。如果反斜杠出现在末尾，即 `\` 是字符串的最后一个字符，那么这种情况通常被视为格式不完整或非法输入，具体处理方式取决于程序设计：可以将其按原样（`\`）追加到结果中（宽容处理），或者直接报错并停止解析（严格处理）。通过这种机制，所有具有特殊意义的转义序列都会被解析和替换，而所有普通字符则被保留，最终得到一个正确反映原始数据的字符串。

```cpp
for (size_t i = 0; i < str.size(); ++i) {
    if (str[i] != '\\') {
        result.push_back(str[i]);
    } else {
        if (i + 1 < str.size()) {
            char next = str[++i];
            switch (next) {
                case '\\': result.push_back('\\'); break;
                case '"':  result.push_back('"');  break;
                case 'n':  result.push_back('\n'); break;
                default:   result.push_back(next); break;
            }
        }
    }
}
```

综上，这样的代码就足够了：

```cpp
namespace {
    std::string unquoteAndUnescape(const std::string_view& sv) {
        const size_t n = sv.size();
        if (n < 2)
            return std::string{sv};
        
        const char first = sv.front();
        const char last = sv.back();
        
        // 检查是否被引号包围
        if (!((first == '"' && last == '"') || 
              (first == '\'' && last == '\'')))
            return std::string{sv};
        
        // 提取引号内的内容
        std::string_view inner{sv.data() + 1, n - 2};
        std::string out;
        out.reserve(inner.size());
        
        // 处理转义序列
        for (size_t i = 0; i < inner.size(); ++i) {
            char c = inner[i];
            
            if (c == '\\' && i + 1 < inner.size()) {
                char next = inner[i + 1];
                switch (next) {
                    case 'n':  out.push_back('\n'); break;
                    case 't':  out.push_back('\t'); break;
                    case '\\': out.push_back('\\'); break;
                    case '"':  out.push_back('"'); break;
                    case '\'': out.push_back('\''); break;
                    default:   out.push_back(next); break;
                }
                ++i; // 跳过下一个字符
            } else {
                out.push_back(c);
            }
        }
        
        return out;
    }
}
```

### 📝 最终版实现

```cpp
bool IniParser::consume_line(const std::string_view& line,
                             std::string& current_section) {
    const auto preprocessed_line = cxx_utils::string::trim_view(line);
    if (preprocessed_line.empty()) {
        return true;
    }
    
    if (is_comments(preprocessed_line)) {
        return true;
    }
    
    if (preprocessed_line.front() == '[') {
        auto end_section_pos = preprocessed_line.find(']');
        if (end_section_pos == std::string::npos) {
            return false;
        }
        
        auto section_sv = preprocessed_line.substr(1, end_section_pos - 1);
        section_sv = string::trim_view(section_sv);
        
        if (end_section_pos + 1 < preprocessed_line.size()) {
            for (size_t i = end_section_pos + 1; i < preprocessed_line.size(); ++i) {
                const char c = preprocessed_line[i];
                if ((c == ';' || c == '#') && 
                    !isPositionInsideQuotes(preprocessed_line, i)) {
                    break;
                }
                if (!std::isspace(static_cast<unsigned char>(c))) {
                    return false;
                }
            }
        }
        
        current_section = section_sv;
        if (m_data.find(current_section) == m_data.end()) {
            m_data.emplace(current_section, ini_section_t{});
        }
        return true;
    }
    
    size_t comment_pos = std::string::npos;
    for (size_t i = 0; i < preprocessed_line.size(); ++i) {
        if ((preprocessed_line[i] == ';' || preprocessed_line[i] == '#')
            && !isPositionInsideQuotes(preprocessed_line, i)) {
            comment_pos = i;
            break;
        }
    }
    
    const auto without_comment = (comment_pos == std::string::npos) 
        ? preprocessed_line 
        : preprocessed_line.substr(0, comment_pos);
    
    const auto splited_kv = string::splits(without_comment, "=");
    if (splited_kv.size() < 2) {
        return false;
    }
    
    std::string key{string::trim_view(splited_kv[0])};
    
    const auto pos = without_comment.find('=');
    if (pos == std::string::npos) {
        return false;
    }
    
    // 最后更新：使用unquoteAndUnescape处理值
    std::string value = unquoteAndUnescape(
        string::trim_src(without_comment.substr(pos + 1))
    );
    
    if (key.empty()) {
        return false;
    }
    
    m_data[current_section][key] = value;
    return true;
}
```

## 完整实现代码

经过5次迭代，我们得到了完整的实现。这是最终的产品代码，其他的接口供同志们进行练习，这里不再单独讲解。

```cpp
#include "ini_parse.h"
#include "string_splits.h"
#include "string_trim.h"
#include <optional>
#include <string>
#include <string_view>

namespace {
// 辅助函数1: 检查是否为注释行 (迭代1产生)
constexpr inline bool is_comments(const std::string_view sv) {
    const char is_comments_ch = sv.front();
    if (is_comments_ch == ';' || is_comments_ch == '#') {
        return true;
    }
    return false;
}

// 辅助函数2: 检查位置是否在引号内 (迭代4产生)
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

// 辅助函数3: 移除引号并处理转义 (迭代5产生)
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

// 核心函数：逐行解析
bool IniParser::consume_line(const std::string_view& line,
                             std::string& current_section) {
    const auto preprocessed_line = cxx_utils::string::trim_view(line);
    if (preprocessed_line.empty()) {
        return true;
    }

    // 检查是否为注释行
    if (is_comments(preprocessed_line)) {
        return true;
    }

    // 检查是否为section
    if (preprocessed_line.front() == '[') {
        auto end_section_pos = preprocessed_line.find(']');
        if (end_section_pos == std::string::npos) {
            return false; // 没有闭合的]
        }

        auto section_sv = preprocessed_line.substr(1, end_section_pos - 1);
        section_sv = string::trim_view(section_sv);

        // 检查]后面是否只有空白或注释
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
        return true;
    }

    // 查找行内注释的位置（不在引号内）
    size_t comment_pos = std::string::npos;
    for (size_t i = 0; i < preprocessed_line.size(); ++i) {
        if ((preprocessed_line[i] == ';' || preprocessed_line[i] == '#')
            && !isPositionInsideQuotes(preprocessed_line, i)) {
            comment_pos = i;
            break;
        }
    }

    const auto without_comment = (comment_pos == std::string::npos) 
        ? preprocessed_line 
        : preprocessed_line.substr(0, comment_pos);

    // 分割键值对
    const auto splited_kv = string::splits(without_comment, "=");

    if (splited_kv.size() < 2) {
        return false; // 没有等号
    }

    std::string key { string::trim_view(splited_kv[0]) };

    // 提取值（可能包含=）
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

// 解析整个INI文件
bool IniParser::parse(const std::string_view view) {
    // 重置所有数据
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

// 获取配置值（返回optional）
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

// 获取配置值（带默认值）
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

// 检查键是否存在
bool IniParser::has(const std::string& section,
                    const std::string& key) {
    auto sit = m_data.find(section);
    if (sit == m_data.end())
        return false;
    return sit->second.find(key) != sit->second.end();
}

}
```

