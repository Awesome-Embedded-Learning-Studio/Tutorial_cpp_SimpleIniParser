add_library(string_splits string_splits.cpp string_trim.cpp)
target_include_directories(string_splits PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
