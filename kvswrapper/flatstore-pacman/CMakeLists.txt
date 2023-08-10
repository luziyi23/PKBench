cmake_minimum_required(VERSION 3.14)
project(PACMAN) 
set(CMAKE_CXX_STANDARD 17)

# configurations

# config.h
set(CMAKE_BUILD_TYPE "Release")
set(USE_NUMA_NODE 0 CACHE STRING "To use which numa node")
option(IDX_PERSISTENT "IDX_PERSISTENT" ON)
option(LOG_PERSISTENT "LOG_PERSISTENT" ON)
set(INDEX_TYPE 2 CACHE STRING "Index type of DB") # 1 CCEH 2 FastFair 3 Masstree
if (INDEX_TYPE EQUAL 3) # MASSTREE
  set(IDX_PERSISTENT OFF)
endif()

option(LOG_BATCHING "LOG_BATCHING to simulate FlatStore's batching (if LOG_PERSISTENT)" OFF)

option(REDUCE_PM_ACCESS "REDUCE_PM_ACCESS" OFF)
option(HOT_COLD_SEPARATE "HOT_COLD_SEPARATE" OFF)
option(GC_SHORTCUT "GC_SHORTCUT" OFF)
option(BATCH_COMPACTION "BATCH_COMPACTION" OFF)

option(PACMAN "PACMAN" ON)
SET(USE_PMDK OFF)
SET(LOG_BATCHING OFF)
set(PACMAN OFF)
if (PACMAN)
  set(REDUCE_PM_ACCESS ON)
  set(HOT_COLD_SEPARATE ON)
  set(BATCH_COMPACTION ON)
  if (INDEX_TYPE EQUAL 1 AND NOT IDX_PERSISTENT)
    set(GC_SHORTCUT OFF)
  else()
    set(GC_SHORTCUT ON)
  endif()
endif()

message(STATUS "SET PACMAN ${PACMAN}")
message(STATUS "SET USE_NUMA_NODE ${USE_NUMA_NODE}")
message(STATUS "SET INDEX_TYPE ${INDEX_TYPE}")
message(STATUS "SET IDX_PERSISTENT ${IDX_PERSISTENT}")
message(STATUS "SET LOG_PERSISTENT ${LOG_PERSISTENT}")
message(STATUS "SET LOG_BATCHING ${LOG_BATCHING}")
message(STATUS "SET REDUCE_PM_ACCESS ${REDUCE_PM_ACCESS}")
message(STATUS "SET HOT_COLD_SEPARATE ${HOT_COLD_SEPARATE}")
message(STATUS "SET GC_SHORTCUT ${GC_SHORTCUT}")
message(STATUS "SET BATCH_COMPACTION ${BATCH_COMPACTION}")

configure_file(
  "${PROJECT_SOURCE_DIR}/include/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
)

############
option(LOGGING "LOGGING" OFF)
#################################################################

find_library(PMEM pmem)
find_library(PMEMOBJ pmemobj)

include_directories(
  "${PROJECT_SOURCE_DIR}/include"
  "${PROJECT_SOURCE_DIR}"
  "${PROJECT_BINARY_DIR}"
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -ggdb -pthread -march=native")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread -g -ggdb -pthread -march=native")

aux_source_directory(${PROJECT_SOURCE_DIR}/db DB_FILES)
aux_source_directory(${PROJECT_SOURCE_DIR}/util UTIL_FILES)
set(DB_FILES ${DB_FILES} ${UTIL_FILES})

# FastFair
add_library(fastfair SHARED ${PROJECT_SOURCE_DIR}/db/index/FAST_FAIR/ff_btree.cpp)

# CCEH
add_library(cceh SHARED ${PROJECT_SOURCE_DIR}/db/index/CCEH/CCEH_MSB.cpp)

# Masstree
aux_source_directory(${PROJECT_SOURCE_DIR}/db/index/masstree MASSTREE_SOURCE_FILES)
add_library(masstree SHARED ${MASSTREE_SOURCE_FILES})
# target_include_directories(masstree ${PROJECT_SOURCE_DIR}/db/index/masstree)
target_compile_options(masstree PRIVATE
          $<$<COMPILE_LANGUAGE:CXX>:-include ${PROJECT_SOURCE_DIR}/db/index/masstree/config.h>)


# wrapper
add_library(flatstore_wrapper SHARED ${DB_FILES} flatstore_wrapper.cpp)
# add_executable(test ${DB_FILES} test.cpp)
target_link_libraries(flatstore_wrapper ${PMEM} ${PMEMOBJ})
if (INDEX_TYPE EQUAL 1)
  target_link_libraries(flatstore_wrapper cceh)
elseif (INDEX_TYPE EQUAL 2)
  target_link_libraries(flatstore_wrapper fastfair)
elseif (INDEX_TYPE EQUAL 3)
  target_link_libraries(flatstore_wrapper masstree)
#   target_link_libraries(test masstree ${PMEM} ${PMEMOBJ})
endif()

