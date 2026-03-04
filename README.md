# Tutorial_cpp_SimpleIniParser

> **现代C++工程实践：手写一个 INI 解析器**

本项目配套视频教程：[现代C++工程实践 - B站](https://space.bilibili.com/294645890/lists/7045956)

---

## 📺 视频教程

### 基础知识

| # | 视频标题 | B站链接 | 状态 |
|---|----------|---------|------|
| 1 | std::string_view | [📺](https://www.bilibili.com/video/BV1c5qDBiExY/) | ✅ |
| 2 | std::optional与错误处理 | [📺](https://www.bilibili.com/video/BV1g9BHBgEFC/) | ✅ |
| 3 | 编写split函数（上） | [📺](https://www.bilibili.com/video/BV1ZUBzBiEtZ/) | ✅ |
| 4 | 编写split函数（下） | [📺](https://www.bilibili.com/video/BV1fmBzBTE95/) | ✅ |
| 5 | CMake速讲 | [📺](https://www.bilibili.com/video/BV1LAvUBEEUR/) | ✅ |
| 6 | trim的实现 | [📺](https://www.bilibili.com/video/BV15TvUB1EiC/) | ✅ |

### IniParser 实战

| # | 视频标题 | B站链接 | 状态 |
|---|----------|---------|------|
| 1 | 从IniParser出发！ | [📺](https://www.bilibili.com/video/BV154qjB8EA5/) | ✅ |
| 2 | 需求细化与接口设计 | [📺](https://www.bilibili.com/video/BV1NVqRBXEem/) | ✅ |
| 3 | 正式手写IniParser（1） | [📺](https://www.bilibili.com/video/BV1s6i7B1ENs/) | ✅ |
| 4 | 正式手写IniParser（2） | [📺](https://www.bilibili.com/video/BV1s6i7B2ESh/) | ✅ |
| 5 | 正式手写IniParser（3） | [📺](https://www.bilibili.com/video/BV1P6i7B2EmH/) | ✅ |
| 6 | 正式手写IniParser（4） | [📺](https://www.bilibili.com/video/BV1A1i7BVELA/) | ✅ |

---

## 📚 学习路线

```
┌─────────────────────────────────────────────────────────┐
│                    IniParser 学习路线                    │
├─────────────────────────────────────────────────────────┤
│                                                         │
│  Step 1: 基础知识准备                                    │
│  ├── std::string_view (字符串视图)                     │
│  ├── std::optional (可选值与错误处理)                   │
│  ├── split/trim 函数实现 (字符串处理)                   │
│  └── CMake 基础                                         │
│                                                         │
│  Step 2: 需求分析与设计                                 │
│  ├── 从 IniParser 出发 (项目介绍)                       │
│  └── 需求细化与接口设计                                 │
│                                                         │
│  Step 3: 实战实现                                       │
│  ├── 正式手写 IniParser (1-4)                           │
│  └── 完整功能实现                                       │
│                                                         │
└─────────────────────────────────────────────────────────┘
```

---

## 🗂️ 项目结构

```
Tutorial_cpp_SimpleIniParser/
├── codes_and_assets/          # 代码和资源文件
├── tutorial/                  # 教程 Markdown 文件
│   ├── index.md              # 教程导航页
│   ├── 从简单的IniParser开启我们的旅程.md
│   ├── 分解需求——刨析split.md
│   ├── 改进我们的split.md
│   └── 实现我们的ini_mappings.md
├── include/                   # 头文件目录
├── src/                       # 源码目录
└── CMakeLists.txt            # CMake 配置文件
```

---

## 🚀 快速开始

### 环境要求

- **编译器**: 支持 C++17 的 GCC 8+ / Clang 7+ / MSVC 2017+
- **构建工具**: CMake 3.10+

### 构建项目

```bash
# 克隆仓库
git clone https://github.com/Awesome-Embedded-Learning-Studio/Tutorial_cpp_SimpleIniParser.git
cd Tutorial_cpp_SimpleIniParser

# 构建
mkdir build && cd build
cmake ..
make

# 运行示例
./iniparser_demo
```

---

## 📖 配套文档

- **主仓库**: [CXXBaseComponents](https://github.com/your-username/Project_CXXBaseComponents)
- **教程文档**: [documentation/tutorial/iniparser/](https://github.com/your-username/Project_CXXBaseComponents/tree/main/documentation/tutorial/iniparser)

---

## 👨‍💻 作者

- **作者**: Charliechen114514
- **B站**: [是的一个城管](https://space.bilibili.com/294645890)
- **组织**: [Awesome-Embedded-Learning-Studio](https://github.com/Awesome-Embedded-Learning-Studio)

---

## 📄 许可证

本项目采用 MIT 许可证 - 详见 [LICENSE](LICENSE) 文件
