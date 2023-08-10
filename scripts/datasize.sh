datasizes=(25000000 50000000 100000000 200000000 400000000 800000000 1600000000 3200000000 6400000000)
# datasize=10000
# 25000000 50000000 100000000 200000000 400000000 800000000 16000000003200000000
for datasize in 25000000 50000000 100000000 1600000000
do
((pool_size=$datasize*16*6))
echo "------------------------------------------------------------------"
echo $datasize
echo $pool_size
echo "------------------------------------------------------------------"
# rm -rf /mnt/pmem/datasize/*
# ./build/benchmark/PKBench  ./build/kvswrapper/flatstore-pacman/libflatstore_wrapper.so -n $datasize -p 20000000 -r 1 -t 24 -pool_path /mnt/pmem/datasize/ -pool_size 8589934592
# rm /mnt/pmem/wkim/* -rf
# ./build/benchmark/PKBench  ./build/kvswrapper/listdb/liblistdb_wrapper.so -n $datasize -p 10000000 -r 1 -t 24 -pool_path /mnt/pmem/pkbench/pmemkv/ -pool_size $pool_size -pcm
# rm -rf /mnt/pmem/helidb/* 
# ./build/benchmark/PKBench  ./build/kvswrapper/fluidkv/libfluidkv_wrapper.so -n $datasize -p 10000000 -r 1 -t 24 -pool_path /mnt/pmem/helidb -pool_size 549755813888 -pcm
rm /mnt/pmem/pkbench/rocksdb/*
./build/benchmark/PKBench  ./build/kvswrapper/rocksdb/librocksdb_wrapper.so -n $datasize -p 10000000 -r 1 -t 24 -pool_path /mnt/pmem/pkbench/rocksdb -pool_size 85899345920 -pcm
done