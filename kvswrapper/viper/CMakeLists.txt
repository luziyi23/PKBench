cmake_minimum_required(VERSION 3.14)
project(viper)

set(CMAKE_CXX_STANDARD 17)
# VIPER
add_library(viper INTERFACE)
target_include_directories(viper INTERFACE $)
target_compile_options(viper INTERFACE -march=native -mclwb)

include_directories(${PROJECT_SOURCE_DIR})
add_library(viper_wrapper SHARED viper_wrapper.cpp)
target_link_libraries(viper_wrapper viper pthread)
add_library(viper_wrapper_int SHARED viper_wrapper_int.cpp viper_wrapper_int.h)
target_link_libraries(viper_wrapper_int viper pthread)