include(FetchContent)


FetchContent_Declare(
  flexspace
  GIT_REPOSITORY https://github.com/flexible-address-space/flexspace.git
)
# target_compile_options(rocksdb PUBLIC -fPIC)
FetchContent_MakeAvailable(flexspace)

add_library(flexdb_wrapper SHARED flexdb_wrapper.cpp)
target_link_libraries(flexdb_wrapper flexdb)