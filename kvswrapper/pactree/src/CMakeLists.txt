file (GLOB pactree_SRCS "*.h" "*.cpp")
add_library(pactree SHARED
    ${pactree_SRCS}
)
target_include_directories(pactree PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
#add_executable (main main.cpp)
target_link_directories(pactree PUBLIC "/opt/intel/oneapi/tbb/2021.6.0/lib/intel64/gcc4.8/")
target_link_libraries (pactree tbb numa jemalloc pdlart pmemobj pmem)
INSTALL(TARGETS pactree
        ARCHIVE DESTINATION ${CMAKE_SOURCE_DIR}
        )
# FetchContent_Declare(
#         pibench
#         GIT_REPOSITORY https://github.com/sfu-dis/pibench.git
#         GIT_TAG master
# )
# if (NOT pibench_POPULATED)
#   FetchContent_Populate(pibench)
#   include_directories(${pibench_SOURCE_DIR}/include)
# endif ()
