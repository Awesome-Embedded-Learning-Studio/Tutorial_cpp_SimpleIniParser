message("Enter to build string utils")

add_library(IniParser ini_parser.cpp)

target_include_directories(IniParser PUBLIC .)
target_link_libraries(IniParser PUBLIC utils)

message("Enter to build string utils finished")
