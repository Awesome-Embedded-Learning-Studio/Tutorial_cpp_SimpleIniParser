# 现代C++工程实践：简单的IniParser2：分解需求与编写split

> 如果您觉得文章写的还可以，可以花费您的一些小时间看看仓库！[Tutorial_cpp_SimpleIniParser](https://github.com/Awesome-Embedded-Learning-Studio/Tutorial_cpp_SimpleIniParser)

上一篇文章中（链接如下），我们讨论了一下起头和Ini的文件约定规范。

> - 知乎：[现代C++工程实践：简单的IniParser 1——从简单的IniParser开启我们的旅程 - 知乎](https://zhuanlan.zhihu.com/p/1981291162045278061)
> - CSDN：[现代C++工程实践：简单的IniParser 1——从简单的IniParser开启我们的旅程 - 知乎](https://blog.csdn.net/charlie114514191/article/details/155691452)
> - Github：[Tutorial_cpp_SimpleIniParser/tutorial/从简单的IniParser开启我们的旅程.md at main · Awesome-Embedded-Learning-Studio/Tutorial_cpp_SimpleIniParser](https://github.com/Awesome-Embedded-Learning-Studio/Tutorial_cpp_SimpleIniParser/blob/main/tutorial/从简单的IniParser开启我们的旅程.md)
> - 公众号：[现代C++工程实践：简单的IniParser ——从简单的IniParser开启我们的旅程](https://mp.weixin.qq.com/s/cCKgDqi8wn8tPKcZIGfoOA)

这里我们要做的就是分解需求，逐步的思考我们要做什么达到我们的目的。对于这种需求，我们发现它可以被描述为一个过程。作为用户，他/她期待的是——我使用一个IniParser可以读取文件，也许未来是内存中的字符串或者是其他来头，总之**得到一个满足Ini文件格式的字符串**，然后我们做的事情是——**逐行解析**这个字符串，这个事情我们需要检查一下文件规范，然后，将结果保存。

很好，我们细化了我们的思路。但是思考的结果就会像树一样蔓延，我们很容易想到，上面这一段话还是不足以指导我们展开行动。

- 我们要如何表达存在一个IniParser呢？这个倒是简单，一个类IniParser就好了
- 下一个问题，他接受什么呢？有人会说直接给一个fstream就好了，但是如果在嵌入式环境中我们想快速测试直接给一个字符串，显然好像再冒出来一个sstream看着有点奇怪，而且，这也就意味着我们要在IniParser中管理输入流的状态，不觉得多管闲事吗？（这个看个人喜好，笔者的意思是——不要在一个职责很明确的类中安插无关的事情）
- 我们如何获取逐行？一个想法就是对流getline，但是我们上面不决定采用getline了；那我们需要编写一个string split，他需要完成对给定的字符进行分割。这是我们这篇博客的主要任务
- 我们要编写一个解析器，逐行的解析我们的状态——确定当前行所在的section，以及对应的Key value是如何的，对于双引号/单引号引的内容要转义。基本上就是我们下一篇博客：Parser的编写的主要内容。
- 结果需要保存，那么，我们构思一下，这种很明显的键值对结果，unordered_map是再合适不过了。

到这里，我们的需求被进一步分解成可分配的任务。如果您是团队合作，这个时候就可以分配任务了！有人负责思考输入，有人负责Parser，有人负责底层库实现，有人负责接口设计。

不过不要着急，我们还需要想一想结果。上面是过程的分解，结果的表现同样重要。用户显然要对着IniParser请求解析和得到解析的结果，所以一下子，两个接口就显而易见了：

```cpp
		/**
		 * @brief parse a view-ini string
		 *
		 * @param view
		 * @return true: parse OK
		 * @return false: parse failed
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
```

什么是string_view，什么是optional，您如果很着急现在就可以去查查看，笔者之后会单开一个基础博客详细聊一聊他们的使用。

Parse接口就是简单的刨析字符串，将结果保存在一个Map里，我们随后使用get方法得到我们感兴趣的键值对。在实际上，需求还会大得多！但是敲定编程思路和接口思考就是这样。

## 从split起步

上面我们说了，split还是很重要的。我们要拿他分解每一行和键值对。所以这一片博客就是做这个事情。

> 相关的代码笔者放到了[Tutorial_cpp_SimpleIniParser/codes_and_assets/string_utils/string_splits at main · Awesome-Embedded-Learning-Studio/Tutorial_cpp_SimpleIniParser](https://github.com/Awesome-Embedded-Learning-Studio/Tutorial_cpp_SimpleIniParser/tree/main/codes_and_assets/string_utils/string_splits)，这个实现是最小的MVP实现，所以谈不上任何程度的健全，后续版本迭代中，您可以自己对自己的实现进行优化（比如说提供给定的字符串而不是字符）

C++中的字符串大家都是自己编写split，实际上如果是正儿八经的工程，笔者建议的不是你自己手搓，别做那种事情，而是优先看看**有没有权威的第三方实现**，但是这里是C++练习，我们还是先试试看自己编写。

```cpp
	/**
	 * @brief split the string into pieces of
	 *
	 * @param src
	 * @param ch
	 * @return std::vector<std::string>
	 */
	std::vector<std::string> splits_v1(
	    const std::string& src, const char ch);
```

这个版本是splits_v1,也就是大家第一反应所编写的。

#### `splits_v1(const std::string& src, const char ch) -> std::vector<std::string>`

```cpp
std::vector<std::string> splits_v1(
    const std::string& src, const char ch) {
    std::vector<std::string> results; // result containers
    static constexpr const int start_pos = 0;

    auto current_positions = src.find(ch, start_pos);
    const size_t str_sz = src.size();
    size_t last_index = start_pos;

    while (current_positions != std::string::npos) {
        results.emplace_back(src.substr(last_index, current_positions - last_index));
        results.emplace_back(1, ch);
        last_index = current_positions + 1;
        current_positions = src.find(ch, last_index);
    }

    results.emplace_back(src.substr(last_index));
    return results;
}
```

下面这段将讲解要点整理成了**连续的文章段落**，可直接黏贴进博客：

------

这一版 `splits_v1` 的实现逻辑十分直接：首先使用 `start_pos` 作为扫描的起始下标，这只是一个简单的风格选择，便于后续阅读，不是必须的。随后通过 `src.find(ch, start_pos)` 查找第一个分隔符的位置，并根据返回值进入解析循环。

在循环体中，核心步骤是使用 `src.substr(last_index, current_pos - last_index)` 取出分隔符前的内容片段。这段片段有可能为空（例如字符串以分隔符开头），但依然会被如实保留。随后，代码调用 `results.emplace_back(1, ch)`，将分隔符本身作为长度为 1 的 `std::string` 插入结果数组中。这意味着最终的拆分结果不仅包含文本片段，也包含原始分隔符。接着更新 `last_index = current_pos + 1`，继续向后查找下一个分隔符。

当循环结束后，说明已经没有更多分隔符，此时还需将最后一段从 `last_index` 到字符串结尾的内容加入结果向量中。这段内容同样可能为空，例如输入字符串以分隔符结尾。

从内存管理角度来看，整个函数返回一个 `std::vector<std::string>`，每一次 `substr` 和 `emplace_back` 都会进行拷贝与分配，虽然安全但开销不可忽视。时间复杂度理论上为 O(n)，因为 `find` 会从当前位置继续搜索，不会重复扫描；但由于存在多次内存分配，真实性能会稍逊于纯指针扫描版本。

需要特别注意的是：这个实现**会保留分隔符本身**作为独立的结果元素。如果你在后续处理中并不需要保留分隔符，那么只需要删掉 `results.emplace_back(1, ch)` 这一行即可。

下面给出几个示例，帮助直观理解这种拆分方式的行为特征：

- `src = "a,b,c"`, `ch = ','` → `["a", ",", "b", ",", "c"]`
- `src = ",a,"` → `["", ",", "a", ",", ""]`
- `src = ""` → `[""]`（注意最后仍会产生一个空字符串片段）

------

#### 嗯？是不是有点问题？

不过，随着项目深入，你会逐渐发现 `splits_v1` 虽然简单易懂，但在工程实践中存在不少局限性。首先，它使用的是 `std::string` 来承载每一个分段，这意味着每一次 `substr` 都会触发一次内存分配与拷贝；在长字符串或高频调用场景下，这类分配开销会迅速累积，成为性能隐患。其次，`splits_v1` 无法与更现代的 C++ 字符串视图生态对接，它只能返回真正的字符串副本，而不能直接引用原始输入，这不仅增加了额外的内存占用，也让它难以融入零拷贝处理链路。

除此之外，`splits_v1` 在容量管理方面也比较初级——没有对结果数组进行预留容量，每次 `emplace_back` 都可能触发增长扩容；而扩容本身也是内存分配，对性能并不友好。再者，它的返回类型无法反映源字符串的生命周期约束，导致后续处理无法获得“轻量引用”的优势。

所以，我们就是准备打算请出 `std::string_view` 为核心构建，不再复制任何字符，同时会预计算分隔符数量，并提前 `reserve` 结果向量的容量，从而让整个拆分过程尽可能接近理想状态的 O(n) 且无额外分配。更关键的是，`string_view` 模型让拆分结果可以直接在业务层中零拷贝传播，大幅提升效率。这就是我们下一篇博客的内容：介绍一下string_view，然后，重新聊聊改进版实现的内部细节，并讨论如何利用 `std::string_view` 构建高性能的字符串处理工具链。
