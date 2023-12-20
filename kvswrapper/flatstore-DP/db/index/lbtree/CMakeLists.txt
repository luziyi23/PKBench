cmake_minimum_required(VERSION 3.14)
project(LBTree) 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O3 -std=c++11 -pthread -mrtm -msse4.1 -mavx2")
# LB+tree
include_directories(${PROJECT_SOURCE_DIR}/common ${PROJECT_SOURCE_DIR}/lbtree-src)
aux_source_directory(${PROJECT_SOURCE_DIR}/common LBTREE_SOURCE_FILES)
add_library(lbtree SHARED ${PROJECT_SOURCE_DIR}/lbtree-src/lbtree.cc ${LBTREE_SOURCE_FILES})
target_link_libraries(lbtree pmem pthread)

add_executable(index_test ${PROJECT_SOURCE_DIR}/lbtree-src/lbtree.cc)
target_link_libraries(index_test lbtree pmem pthread)


