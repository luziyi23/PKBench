for thread in 1 4 8 16 24 32
do
echo "------------------------------------------------------------------"
echo $thread
echo "------------------------------------------------------------------"
rm -rf /mnt/pmem/datasize/*
#viper

# ./build/benchmark/PKBench  ./build/kvswrapper/viper/libviper_wrapper_int.so -n $datasize -p 10000 -r 1 -t 1 -pool_path /mnt/pmem/datasize/viper/ -pool_size 1073741824 -pcm
# ./build/benchmark/PKBench  ./build/kvswrapper/viper/libviper_wrapper_int.so -n $datasize -p 10000 -r 1  -t 1 -pool_path /mnt/pmem/datasize/viper/ -pool_size 1073741824 -pcm -skip_load
# ./build/benchmark/PKBench  ./build/kvswrapper/viper/libviper_wrapper_int.so -n $datasize -p 10000000 -r 0 -i 1 -t 1 -pool_path /mnt/pmem/datasize/viper/ -pool_size 1073741824 -pcm -skip_load

#flatstore
# ./build/benchmark/PKBench  ./build/kvswrapper/flatstore-pacman/libflatstore_wrapper.so -n $datasize -p 10000000 -r 1 -t 24 -pool_path /mnt/pmem/pkbench/flatstore/ -pool_size 34359738368 -pcm
./build/benchmark/PKBench  ./build/kvswrapper/flatstore-pacman/libflatstore_wrapper.so -n 200000000 -p 10000000 -r 1 -t $thread -pool_path /mnt/pmem/datasize/ -pool_size 34359738368 -pcm
# ./build/benchmark/PKBench  ./build/kvswrapper/flatstore-pacman/libflatstore_wrapper.so -n $datasize -p 1000000 -r 1 -i 0 -t 1 -pool_path /mnt/pmem/datasize/ -pool_size 34359738368 -skip_load
# ./build/benchmark/PKBench  ./build/kvswrapper/flatstore-pacman/libflatstore-h_wrapper.so -n $datasize -p 10000000 -r 0 -i 1 -t 24 -pool_path /mnt/pmem/datasize/ -pool_size 34359738368 -pcm -skip_load
# ./build/benchmark/PKBench  ./build/kvswrapper/flatstore-pacman/libflatstore-m_wrapper.so -n $datasize -p 10000000 -r 1 -t 24 -pool_path /mnt/pmem/datasize/ -pool_size $pool_size -pcm
# ./build/benchmark/PKBench  ./build/kvswrapper/flatstore-pacman/libflatstore-ph_wrapper.so -n $datasize -p 10000000 -r 1 -t 24 -pool_path /mnt/pmem/datasize/ -pool_size $pool_size -pcm
# ./build/benchmark/PKBench  ./build/kvswrapper/flatstore-pacman/libflatstore-ff_wrapper.so -n $datasize -p 10000000 -r 1 -t 24 -pool_path /mnt/pmem/datasize/ -pool_size $pool_size -pcm


#rocksdb
# ./build/benchmark/PKBench  ./build/kvswrapper/rocksdb/librocksdb_wrapper.so -n 200000000 -p 10000000 -r 1 -t $thread -pool_path /mnt/pmem/datasize -pool_size 85899345920 -pcm

#listdb
# rm /mnt/pmem/wkim/* -rf
# ./build/benchmark/PKBench  ./build/kvswrapper/listdb/liblistdb_wrapper.so -n 200000000 -p 10000000 -r 1 -t 4 -pool_path /mnt/pmem/pkbench/pmemkv/ -pool_size 85899345920 -pcm
done