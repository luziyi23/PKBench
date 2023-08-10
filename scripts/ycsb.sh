# 1 4 8 16 24 #
for thread_num in 16 #1 4 8 16 24 32
do
# workload="-r 0.85 -i 0.15 -t $thread_num -distribution zipfian -scan_size 50 -skew=0.06"
workload="-r 0.06 -i 0.94 -t $thread_num -distribution uniform"
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