add_library(IniParser ini_parse.cpp)
target_link_libraries(IniParser PRIVATE string_splits)
target_include_directories(IniParser PUBLIC .)


