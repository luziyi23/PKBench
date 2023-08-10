#!/bin/bash

blocksizes=("1mb" "64b" "128b" "256b" "512b" "1024b" "2kb" "4kb" "8kb" "16kb" "32kb" "64kb" "128kb" "256kb" "512kb" "1mb")
pm_dir="/mnt/optane-ssd"
threadnum=1
benchmark="randwrite"
for blocksize in ${blocksizes[@]}
do
echo "-----------------$blocksize-------------------------"
# rm $pm_dir/mytest*
sync
echo 3 > /proc/sys/vm/drop_caches 
fio -directory=$pm_dir -direct=0 -iodepth 1 -thread -rw=$benchmark -ioengine=sync -bs=$blocksize -size=10G -numjobs=$threadnum -runtime=180 -group_reporting -name=mytest
# sync
# echo 3 > /proc/sys/vm/drop_caches 
# fio -directory=$pm_dir -direct=1 -iodepth 1 -thread -rw=$benchmark -ioengine=libpmem -bs=$blocksize -size=10G -numjobs=$threadnum -runtime=180 -group_reporting -name=mytest
echo "---------------------------------------------------"
done


#cat xxx.txt | grep -e "bw=" -e "----------"