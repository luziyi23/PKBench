cmake_minimum_required(VERSION 3.24)
project(rocksdbwrapper)
# include(FetchContent)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

SET(CMAKE_BUILD_TYPE Release)
# SET(WITH_LIBURING OFF)
# SET(WITH_TESTS OFF)
# SET(WITH_TOOLS OFF)
# SET(WITH_BENCHMARK_TOOLS OFF)
# SET(WITH_CORE_TOOLS OFF)
# SET(WITH_ALL_TESTS OFF)

# FetchContent_Declare(
#   rocksdb
#   GIT_REPOSITORY https://github.com/facebook/rocksdb.git
#   GIT_TAG v7.3.1
# )
# SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

# FetchContent_MakeAvailable(rocksdb)
# find_library(ROCKSDB_LIB rocksdb)
execute_process(COMMAND make shared_lib ROCKSDB_ON_DCPMM=1 -j8
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/pmem-rocksdb
) 

# add_subdirectory(pmem-rocksdb)
add_library(pmem-rocksdb SHARED IMPORTED)
set_target_properties(pmem-rocksdb PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/pmem-rocksdb/librocksdb.so)
include_directories(${PROJECT_SOURCE_DIR}/pmem-rocksdb/include)
find_library(LZ4_LIB lz4)
add_library(rocksdb_wrapper SHARED rocksdb_wrapper.cpp)
target_link_libraries(rocksdb_wrapper pmem-rocksdb ${LZ4_LIB} -lpthread -lz -lsnappy -lbz2 -lzstd -ldl)