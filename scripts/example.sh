# viper
# export LD_PRELOAD=/mnt/ubuntu-20-10/usr/lib/x86_64-linux-gnu/libtbb.so.2:$LD_PRELOAD
# rm /mnt/pmem/pkbench/viper/ -r
# ./build/benchmark/PKBench  ./build/kvswrapper/viper/libviper_wrapper.so -n 200000000 -p 200000000 -r 1 -t 32 -pool_path /mnt/pmem/pkbench/viper/ -pool_size 8589934592


# flatstore-ff and -m
# rm /mnt/pmem/pkbench/flatstore/*
# ./build/benchmark/PKBench ./build/kvswrapper/flatstore-pacman/libflatstore_wrapper.so -v 8 -n 20000 -p 20000 -r 1 -t 1 -pool_path /mnt/pmem/pkbench/flatstore/ -pool_size 8589934592 -pcm
# ./build/benchmark/PKBench  ./build/kvswrapper/flatstore-m/libflatstore_m_wrapper.so -v 8 -n 200000 -p 200000 -r 1 -t 1 -pool_path /mnt/pmem/pkbench/flatstore/ -pool_size 8589934592 -pcm


# pmemkv
# export LD_PRELOAD=/mnt/ubuntu-20-10/home/lzy/HybridKV/pkbench/build/_deps/pmemkv-build/libpmemkv.so.1:$LD_PRELOAD
# rm /mnt/pmem/pkbench/pmemkv/*
# ./build/benchmark/PKBench  ./build/kvswrapper/pmemkv/libpmemkv_wrapper.so -n 200000000 -p 200000000 -r 1 -t 32 -pool_path /mnt/pmem/pkbench/pmemkv/ -pool_size 85899345920


# listdb
# export LD_PRELOAD=/usr/lib/x86_64-linux-gnu/libjemalloc.so.2:$LD_PRELOAD
# rm /mnt/pmem/wkim/* -rf
# ./build/benchmark/PKBench  ./build/kvswrapper/listdb/liblistdb_wrapper.so -n 200000000 -p 10000000 -r 1 -t 24 -pool_path /mnt/pmem/pkbench/pmemkv/ -pool_size 85899345920 -pcm
# ./build/benchmark/recovery_test  ./build/kvswrapper/listdb/liblistdb_wrapper.so -n 200000000 -p 10000000 -r 1 -t 24 -pool_path /mnt/pmem/pkbench/pmemkv/ -pool_size 85899345920 -pcm


# rocksdb 
# rm /mnt/pmem/pkbench/rocksdb/*
# ./build/benchmark/PKBench  ./build/kvswrapper/rocksdb/librocksdb_wrapper.so -n 25000000 -p 1000000 -r 1 -t 1 -pool_path /mnt/pmem/pkbench/rocksdb -pool_size 85899345920 -pcm

# 549755813888
# fluidkv
# rm /mnt/pmem/pkbench/fluidkv/*
# ./build/benchmark/PKBench  ./build/kvswrapper/fluidkv/libfluidkv_wrapper.so -v 8 -n 200000000 -p 200000000 -r 1 -t 4 -pool_path /mnt/pmem/pkbench/fluidkv -pool_size 17179869184 -pcm


#pactree
# pactree can only recover after the process finish and reopen.
# rm /mnt/pmem/pkbench/pactree/*
# ./build/benchmark/PKBench ./build/kvswrapper/pactree/libpactree_pibench_wrapper.so -n 200000000 -p 1000000 -r 1 -u 0 -t 1 -pool_path /mnt/pmem/pkbench/pactree -pool_size 17179869184 -pcm
# ./build/benchmark/PKBench ./build/kvswrapper/pactree/libpactree_pibench_wrapper.so -n 200000000 -p 1000000 -r 1 -u 0 -t 1 -pool_path /mnt/pmem/pkbench/pactree -pool_size 17179869184 -pcm -skip_load
