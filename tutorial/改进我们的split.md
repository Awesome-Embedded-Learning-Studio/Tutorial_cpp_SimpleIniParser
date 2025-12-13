# 现代C++工程实践：简单的IniParser3——改进我们的split

## 前言

上一篇博客我们提到了说我们是要改进咱们的split的，传送门在下面：

- CSDN：[现代C++工程实践：简单的IniParser2：分解需求与编写split-CSDN博客](https://blog.csdn.net/charlie114514191/article/details/155731063)
- 知乎：[现代C++工程实践：简单的IniParser2：分解需求与编写split - 老老老陈醋的文章 - 知乎](https://zhuanlan.zhihu.com/p/1981659758034437080)
- Github: [Awesome-Embedded-Learning-Studio/Tutorial_cpp_SimpleIniParser: 这是我们C++工程化开始的旅程！手搓一个最简单的Ini分析器！This is the beginning of our journey in C++ engineering! Handcrafting the simplest INI parser!](https://github.com/Awesome-Embedded-Learning-Studio/Tutorial_cpp_SimpleIniParser)

笔者这里单独开了一个博客全面的介绍了`std::string_view`。

- [简说C++17新东西：string_view-CSDN博客](https://blog.csdn.net/charlie114514191/article/details/155843231)
- [C++17: std::string_view 全攻略 - 老老老陈醋的文章 - 知乎](https://zhuanlan.zhihu.com/p/1982745462256714113)

看这篇博客之前，单独看看string_view。

## 下面这个改进对吗

下面这个片段是笔者一开始写出来的，各位看官阅读一下：

```cpp
std::vector<std::string_view> splits_v2(
    const std::string& src, const char ch) {

    std::vector<std::string_view> results;

    if (src.empty()) {
        return results;
    }

    auto current_positions = src.find(ch, 0);
    const size_t str_sz = src.size();
    size_t last_index = 0;

    while (current_positions != std::string::npos) {
        results.emplace_back(src.substr(last_index, current_positions - last_index));
        results.emplace_back(src.substr(current_positions, 1));

        last_index = current_positions + 1;
        current_positions = src.find(ch, current_positions + 1);
    }

    results.emplace_back(src.substr(last_index));
    return results;
}
```

看出来问题了吗？

#### 关键问题： `substr()` 返回的是新的 `std::string`

`std::string::substr()` 不是返回视图，它是 **拷贝构造一个新的字符串**：

```cpp
std::string substr(pos, count);
```

而你的函数返回的是：

```cpp
std::vector<std::string_view>
```

意味着如下情况会发生：

1. `src.substr(...)` 创建了一个**临时的 std::string 对象**
2. `string_view` 绑定到临时对象的内部 `buffer`
3. 临时对象生命周期仅存活到当前表达式结束
4. `string_view` 中的 `ptr` 立即悬空（dangling pointer）
5. 返回后访问 view → **UB（未定义行为）直接爆炸**

也就是说，这个函数看起来运行正常，但返回的数据其实**全部悬空**。所以笔者在Release模式下就惊喜的发现了一堆bug：包括随机数据和直接悬空的问题

这是一种非常常见的 “**临时对象挂掉，string_view 变尸体**” 的经典错误。

------

## 第二版：问题是如何被修复的？

来看修复后的版本：

```cpp
std::vector<std::string_view> splits_v2_fixed(
    const std::string& src, const char ch) {

    std::vector<std::string_view> results;
    if (src.empty()) {
        return results;
    }

    std::string_view src_view(src);  // ⭐ 关键修复点：构造一个稳定的 view

    const size_t delim_count = std::count(src.begin(), src.end(), ch);
    results.reserve(delim_count * 2 + 1);

    size_t last_index = 0;
    size_t current_positions = src.find(ch, last_index);

    while (current_positions != std::string::npos) {

        results.emplace_back(
            src_view.substr(last_index, current_positions - last_index));

        results.emplace_back(
            src_view.substr(current_positions, 1));

        last_index = current_positions + 1;
        current_positions = src.find(ch, last_index);
    }

    results.emplace_back(src_view.substr(last_index));

    return results;
}
```

#### 修复的核心点：使用原始 `src` 构造 `string_view` 作为根

```cpp
std::string_view src_view(src);
```

这样带来两个关键改进：

##### **1. substr() 变成了 `"视图切片"`，不是 `"创建副本"`**

`string_view::substr()` 的实现机制：

- 不会创建新的字符串
- 只计算新的 offset + length
- 返回的 `string_view` 始终指向原始 `src` 的内存区域

源码层面类似：

```cpp
return string_view(this->data() + pos, count);
```

你再怎么分割，它都只是“原文的一块切片”，不会发生内存复制，也不会有临时对象。

##### **2. 原始 `src` 在整个函数调用外都是稳定对象**

你传进来的是：

```cpp
const std::string& src
```

只要调用者保证 `src` 的生命周期≥返回的 vector 的使用生命周期，那么：

- 所有 view 保证不悬空
- 性能更高（完全无拷贝）
- 内存占用更小

现在这个split就被改进了！我们马上就能跑步进入编写一个真正的split了！





