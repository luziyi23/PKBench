# 1 4 8 16 24 #
for thread_num in 16 #1 4 8 16 24 32
do
# workload="-r 0.5 -u 0.5 -t $thread_num -distribution zipfian" # ycsb
# workload="-r 0.95 -u 0.05 -t $thread_num -distribution zipfian" # ycsb
# workload="-r 1 -t $thread_num -distribution zipfian" # ycsb
# workload="-r 0.9 -i 0.1 -t $thread_num -distribution latest" # ycsb
# workload="-r 0.85 -i 0.15 -t $thread_num -distribution zipfian -skew=0.06" # twitter 1
# workload="-r 0.06 -i 0.94 -t $thread_num -distribution uniform" # twitter 2
echo "------------------------------------------------------------------"
echo $workload
echo "------------------------------------------------------------------"

# echo "******************************"
# echo FluidKV
# echo "******************************"
# rm -rf /mnt/pmem/ycsb/* 
# ./build/benchmark/PKBench  ./build/kvswrapper/fluidkv/libfluidkv_wrapper.so -n 200000000 -p 10000000 $workload -pool_path /mnt/pmem/ycsb -pool_size 17179869184 -pcm
echo "******************************"
echo Pactree
echo "******************************"
rm -rf /mnt/pmem/ycsb/* 
./build/benchmark/PKBench ./build/kvswrapper/pactree/libpactree_pibench_wrapper.so -n 200000000 -p 10000000 $workload -pool_path /mnt/pmem/ycsb -pool_size 17179869184 -pcm
# echo "******************************"
# echo Flatstore-ff
# echo "******************************"
# rm -rf /mnt/pmem/ycsb/* 
# ./build/benchmark/PKBench-flatstore  ./build/kvswrapper/flatstore-pacman/libflatstore_wrapper.so -n 200000000 -p 10000000 $workload -pool_path /mnt/pmem/ycsb -pool_size 8589934592 -pcm
# echo "******************************"
# echo ListDB
# echo "******************************"
# rm -rf /mnt/pmem/wkim/*
# ./build/benchmark/PKBench  ./build/kvswrapper/listdb/liblistdb_wrapper.so -n 200000000 -p 10000000 $workload -pool_path /mnt/pmem/ycsb -pool_size 17179869184 -pcm
done