cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(listdb)

message("${CMAKE_C_FLAGS}")
message("${CMAKE_CXX_FLAGS}")

set(CMAKE_CXX_COMPILER "g++")
#set(CMAKE_VERBOSE_MAKEFILE ON)

option(DEBUG "build debug mode." OFF)
option(STRING_KEY "string key mode." OFF)
option(WISCKEY "Store values in Wisckey manner." OFF)
option(SKIPLIST_CACHE "SkipListCache." OFF)
SET(STRING_KEY OFF)
SET(WISCKEY OFF)
if(DEBUG)
  message("[O] DEBUG MODE.")
  set(CMAKE_C_FLAGS "-Wall -Wsign-compare -g ${CMAKE_C_FLAGS}")
else()
  message("[X] RELEASE MODE: DEBUG disabled.")
  #set(CMAKE_C_FLAGS "-Wall -Wsign-compare -pg")
  set(CMAKE_C_FLAGS "-Wall -Wsign-compare -O3 -g -DNDEBUG ${CMAKE_C_FLAGS}")
endif(DEBUG)

if(STRING_KEY)
  message("[O] STRING_KEY ENABLED.")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLISTDB_STRING_KEY")
else()
  message("[X] STRING_KEY disabled.")
endif(STRING_KEY)

if(WISCKEY)
  message("[O] WISCKEY ENABLED.")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLISTDB_WISCKEY")
else()
  message("[X] WISCKEY disabled.")
endif(WISCKEY)

if(SKIPLIST_CACHE)
  message("[O] SKIPLIST_CACHE ENABLED.")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLISTDB_SKIPLIST_CACHE")
else()
  message("[X] SKIPLIST_CACHE disabled.")
endif(SKIPLIST_CACHE)

if(WAL)
  message("[O] WAL ENABLED.")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLISTDB_WAL")
else()
  message("[X] WAL disabled.")
endif(WAL)

##
# GFLAGS
find_package(gflags)
if(gflags_FOUND)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DGFLAGS")
endif()

##
# Compiler options
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wfatal-errors ")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++17 -march=native ")

message("${CMAKE_C_FLAGS}")
message("${CMAKE_CXX_FLAGS}")

##
# Link options
set(LINK_FLAGS "-lpthread")

##
# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR})
#include_directories(/home/wkim/Projects/libcds-2.3.3)
add_definitions(-DLISTDB_L0_CACHE)
##
# liblistdb
#set(LISTDB_SRC_DIR ${PROJECT_SOURCE_DIR}/listdb)
##file(GLOB_RECURSE COMMON_FILE ${LISTDB_SRC_DIR}/*.cc)
##list(REMOVE_ITEM COMMON_FILE *_test.cc)
##message(${COMMON_FILE})
##add_library(listdb SHARED ${COMMON_FIlistdb
add_library(listdb SHARED
  #listdb/lsm/table.cc
  #listdb/listdb.cc
  listdb/lib/murmur3.cc
  listdb/lib/sha1.cc
  )
target_link_libraries(listdb
  pthread
  pmemobj
  pmempool
  numa
  jemalloc
  )

  add_library(listdb_wrapper SHARED listdb_wrapper.cpp)
  target_link_libraries(listdb_wrapper
      listdb
      pthread
      numa
      jemalloc
      pmemobj
      stdc++fs
      tbb
      )

# test
#file(GLOB_RECURSE test_srcs ${LISTDB_SRC_DIR}/*_test.cc)
# if(STRING_KEY)
# set(test_srcs
#   listdb/pmem/pmem_test.cc
#   listdb/lib/numa_test.cc
#   listdb/core/skiplist_cache_test.cc
#   )
# else()
# set(test_srcs
#   listdb/pmem/pmem_test.cc
#   listdb/lib/numa_test.cc
#   listdb/db_client_test.cc
#   listdb/index/braided_pmem_skiplist_test.cc
#   listdb/core/skiplist_cache_test.cc
#   )
# endif(STRING_KEY)
# foreach (test_src ${test_srcs})
#   get_filename_component(test_name ${test_src} NAME_WE)
#   add_executable(${test_name} ${test_src}
#     #listdb/listdb.cc
#     )
#   target_link_libraries(${test_name}
#     listdb
#     pthread
#     pmemobj
#     pmempool
#     numa
#     jemalloc
#     stdc++fs
#     )
#   #add_executable(${test_name} ${test_src}
#   #  listdb/user_key.cc
#   #  listdb/lsm/table.cc)
#   #target_link_libraries(${test_name} listdb)
#   #target_link_libraries(${test_name}
#   #  pthread
#   #  numa
#   #  jemalloc
#   #  )
# endforeach()

# ubench
#file(GLOB_RECURSE ubench_srcs ${LISTDB_SRC_DIR}/ubench/*.cc)
# set(ubench_srcs
#   listdb/ubench/eee.cc
#   listdb/ubench/rw_ratio.cc
#   listdb/ubench/tune_l1.cc
#   listdb/ubench/recovery_test.cc
#   listdb/ubench/string_kv_test.cc
#   listdb/ubench/ycsb_index_microbench.cc
#   )
# foreach (ubench_src ${ubench_srcs})
#   get_filename_component(ubench_name ${ubench_src} NAME_WE)
#   add_executable(${ubench_name} ${ubench_src}
#     #listdb/listdb.cc
#     #listdb/lsm/table.cc
#     )
#   #target_link_libraries(${ubench_name} listdb)
#   target_link_libraries(${ubench_name}
#     listdb
#     pthread
#     numa
#     jemalloc
#     pmemobj
#     stdc++fs
#     tbb
#     gflags
#     #/home/wkim/Projects/libcds-2.3.3/build/bin/libcds.so
#     )
#   if(gflags_FOUND)
#     target_link_libraries(${ubench_name} ${gflags_LIBRARIES})
#   endif()
#   #target_include_directories(${ubench_name} PUBLIC
#   #  /home/wkim/Projects/libcds-2.3.3
#   #  )
# endforeach()

# db_bench
set(db_bench_srcs
  listdb/tools/db_bench.cc
)
foreach (db_bench_src ${db_bench_srcs})
  get_filename_component(db_bench_name ${db_bench_src} NAME_WE)
  add_executable(${db_bench_name} ${db_bench_src}
    )
  target_link_libraries(${db_bench_name}
    listdb
    pthread
    numa
    jemalloc
    pmemobj
    stdc++fs
    tbb
    )
  if(gflags_FOUND)
    target_link_libraries(${db_bench_name} ${gflags_LIBRARIES})
  endif()
endforeach()

