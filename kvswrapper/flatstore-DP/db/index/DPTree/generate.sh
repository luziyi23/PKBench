binary_path="../../.."

eval "rm -rf build && mkdir build && cd build && cmake .. && make && mv libdptree_pibench_wrapper.so ${binary_path}/libdptree_pmem.so"
eval "cmake -DVAR_KEY=1 .. && make && mv libdptree_pibench_wrapper.so ${binary_path}/libdptree_pmem_varkey.so"

echo "DPTree wrappers built complete."