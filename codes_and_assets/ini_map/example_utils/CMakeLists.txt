add_subdirectory(privates)

add_library(ExampleUtils STATIC banner.cpp)
target_include_directories(ExampleUtils PUBLIC .)
target_link_libraries(ExampleUtils PRIVATE ExampleUtilsPrivate)
