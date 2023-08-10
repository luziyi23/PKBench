#!/bin/bash

blocksize="256b"
pm_dir="/mnt/pmem"
threadnum+=(`seq 1 16`)
threadnum+=(32 24)
# benchmark="randread"
for thread in ${threadnum[@]}
do
echo "-----------------$thread-------------------------"
# rm $pm_dir/mytest*
sync
echo 3 > /proc/sys/vm/drop_caches 
fio -directory=$pm_dir -direct=1 -iodepth 1 -thread -rw=write -ioengine=libpmem -bs=$blocksize -size=10G -numjobs=$thread -runtime=180 -group_reporting -name=mytest
sync
echo 3 > /proc/sys/vm/drop_caches 
fio -directory=$pm_dir -direct=1 -iodepth 1 -thread -rw=read -ioengine=libpmem -bs=$blocksize -size=10G -numjobs=$thread -runtime=180 -group_reporting -name=mytest
echo "---------------------------------------------------"
done


#cat xxx.txt | grep -e "bw=" -e "----------"