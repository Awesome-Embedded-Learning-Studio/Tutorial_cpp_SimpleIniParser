message(STATUS "Configuring the ExampleUtilsPrivate")
add_library(ExampleUtilsPrivate STATIC
            banner/banner_private.cpp 
            banner/banner_private.h)


target_include_directories(ExampleUtilsPrivate PUBLIC .)
message(STATUS "Configuring the ExampleUtilsPrivate Done")

