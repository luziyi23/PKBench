cmake_minimum_required(VERSION 3.14)
project(PKBench)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


option(PKBENCH_BUILD_LEVELDB "build leveldb library" OFF)
option(PKBENCH_BUILD_ROCKSDB "build rocksdb library" ON)
option(PKBENCH_BUILD_VIPER "build viper library" ON)
option(PKBENCH_BUILD_PMEMKV "build pmem_kv library" ON)
option(PKBENCH_BUILD_BULLET "build Bullet library" OFF)
option(PKBENCH_BUILD_FLATSTORE "build flatstore library" ON)
option(PKBENCH_BUILD_LISTDB "build listdb library" ON)
option(PKBENCH_BUILD_FLUIDKV "build FluidKV library" ON)
option(PKBENCH_BUILD_PACTREE "build PACTree library" ON)
option(PKBENCH_BUILD_ERT "build ExtendibleRadixTree library" OFF)
set(PKBENCH_BUILD_LEVELDB OFF)
set(PKBENCH_BUILD_ROCKSDB ON)
set(PKBENCH_BUILD_VIPER ON)
set(PKBENCH_BUILD_PMEMKV OFF)
set(PKBENCH_BUILD_BULLET OFF)
set(PKBENCH_BUILD_FLUIDKV ON)
set(PKBENCH_BUILD_PACTREE ON)
set(PKBENCH_BUILD_ERT OFF)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -g -ggdb -march=native")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -g -ggdb -march=native")
# set(OpenMP_CXX_FLAGS "-fopenmp")
find_package(OpenMP REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/include)

if(PKBENCH_BUILD_LEVELDB)
    add_subdirectory(kvswrapper/leveldb)
endif(PKBENCH_BUILD_LEVELDB)

if(PKBENCH_BUILD_ROCKSDB)
    add_subdirectory(kvswrapper/rocksdb)
endif(PKBENCH_BUILD_ROCKSDB)

if(PKBENCH_BUILD_VIPER)
    add_subdirectory(kvswrapper/viper)
endif(PKBENCH_BUILD_VIPER)

if(PKBENCH_BUILD_PMEMKV)
    add_subdirectory(kvswrapper/pmemkv)
endif(PKBENCH_BUILD_PMEMKV)

if(PKBENCH_BUILD_BULLET)
    add_subdirectory(kvswrapper/bullet)
endif(PKBENCH_BUILD_BULLET)

if(PKBENCH_BUILD_FLATSTORE)
    add_subdirectory(kvswrapper/flatstore-pacman)
    add_subdirectory(kvswrapper/flatstore-m)
endif(PKBENCH_BUILD_FLATSTORE)

if(PKBENCH_BUILD_LISTDB)
    add_subdirectory(kvswrapper/listdb)
endif(PKBENCH_BUILD_LISTDB)

if(PKBENCH_BUILD_FLUIDKV)
    add_subdirectory(kvswrapper/fluidkv)
endif(PKBENCH_BUILD_FLUIDKV)

if(PKBENCH_BUILD_PACTREE)
    add_subdirectory(kvswrapper/pactree)
endif(PKBENCH_BUILD_PACTREE)

if(PKBENCH_BUILD_ERT)
    add_subdirectory(kvswrapper/ExtendibleRadixTree)
endif(PKBENCH_BUILD_ERT)

add_subdirectory(benchmark)



