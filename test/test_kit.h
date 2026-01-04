#pragma once
#include <iostream>

// 测试计数器
static int tests_passed = 0;
static int tests_failed = 0;

#define TEST_CASE(name) std::cout << "\n=== " << name << " ===" << std::endl;

#define ASSERT_EQ(actual, expected, msg)                                       \
  do {                                                                         \
    if ((actual) == (expected)) {                                              \
      tests_passed++;                                                          \
      std::cout << "✓ " << msg << std::endl;                                   \
    } else {                                                                   \
      tests_failed++;                                                          \
      std::cout << "✗ " << msg << "\n  Expected: " << (expected)               \
                << "\n  Got: " << (actual) << std::endl;                       \
    }                                                                          \
  } while (0)

#define ASSERT_TRUE(condition, msg)                                            \
  do {                                                                         \
    if (condition) {                                                           \
      tests_passed++;                                                          \
      std::cout << "✓ " << msg << std::endl;                                   \
    } else {                                                                   \
      tests_failed++;                                                          \
      std::cout << "✗ " << msg << " (condition failed)" << std::endl;          \
    }                                                                          \
  } while (0)

#define ASSERT_FALSE(condition, msg)                                           \
  do {                                                                         \
    if (!(condition)) {                                                        \
      tests_passed++;                                                          \
      std::cout << "✓ " << msg << std::endl;                                   \
    } else {                                                                   \
      tests_failed++;                                                          \
      std::cout << "✗ " << msg << " (expected false)" << std::endl;            \
    }                                                                          \
  } while (0)
