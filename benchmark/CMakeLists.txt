set(pkbench_SRC
    key_generator.cpp
    library_loader.cpp
    benchmark.cpp
    operation_generator.cpp
    value_generator.cpp
)
######################## Intel PCM ########################
include(FetchContent)
FetchContent_Declare(
        pcm
        GIT_REPOSITORY https://github.com/opcm/pcm.git
        GIT_TAG 202205
)
FetchContent_MakeAvailable(pcm)
###########################################################
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_BINARY_DIR}/_deps/pcm-src/src)

add_library(pkbench ${pkbench_SRC})
target_compile_options(pkbench PRIVATE ${OpenMP_CXX_FLAGS})
target_link_libraries(pkbench PRIVATE
    ${OpenMP_CXX_FLAGS}
    PCM_SHARED
    dl)


add_executable(pkbench-bin main.cpp)
target_link_libraries(pkbench-bin pkbench)
set_target_properties(pkbench-bin PROPERTIES OUTPUT_NAME PKBench)
add_executable(recovery_test recovery_test.cpp)
target_link_libraries(recovery_test pkbench)


if(PKBENCH_BUILD_FLATSTORE)
add_executable(PKBench-flatstore main.cpp)
target_link_libraries(PKBench-flatstore pkbench flatstore_wrapper)
target_compile_definitions(PKBench-flatstore PRIVATE FLATSTORE)
add_executable(recovery_test-flatstore recovery_test.cpp)
target_link_libraries(recovery_test-flatstore pkbench)
endif(PKBENCH_BUILD_FLATSTORE)