# PKBench
A benchmarking tool for key-value stores on persistent memory.

Modified based on [PiBench](https://github.com/sfu-dis/pibench) and adapted for key-value stores.

# KVS Support

- [FluidKV](github.com/luziyi23/fluidkv)
- [FlatStore](https://github.com/thustorage/pacman)
- [ListDB](https://github.com/DICL/listdb)
- [PacTree](https://github.com/cosmoss-jigu/pactree)
- [Pmem-RocksDB](https://github.com/pmem/pmem-rocksdb)
- [PmemKV](https://github.com/pmem/pmemkv)
- [Viper](https://github.com/hpides/viper)

The interfaces and tests of Bullet, ERT, and LevelDB are not ready yet. 

# Usage

Compile:
```
$cmake -B build
$cmake --build build -j
```
Usage:
```
$./build/benchmark/PKBench
Usage:
  PiBench [OPTION...] INPUT

      --input arg             Absolute path to library file
  -n, --records arg           Number of records to load (default: 1000000)
  -p, --operations arg        Number of operations to execute (default: 
                              1000000)
  -t, --threads arg           Number of threads to use (default: 1)
  -f, --key_prefix arg        Prefix string prepended to every key 
                              (default: "")
  -k, --key_size arg          Size of keys in Bytes (without prefix) 
                              (default: 8)
  -v, --value_size arg        Size of values in Bytes (default: 8)
  -r, --read_ratio arg        Ratio of read operations (default: 1.000000)
  -i, --insert_ratio arg      Ratio of insert operations (default: 
                              0.000000)
  -u, --update_ratio arg      Ratio of update operations (default: 
                              0.000000)
  -d, --remove_ratio arg      Ratio of remove operations (default: 
                              0.000000)
  -s, --scan_ratio arg        Ratio of scan operations (default: 0.000000)
      --scan_size arg         Number of records to be scanned. (default: 
                              100)
      --sampling_ms arg       Sampling window in milliseconds (default: 
                              1000)
      --distribution arg      Key distribution to use (default: UNIFORM)
      --skew arg              Key distribution skew factor to use (default: 
                              0.990000)
      --seed arg              Seed for random generators (default: 1729)
      --pcm                   Turn on Intel PCM
      --pool_path arg         Path to persistent pool (default: "")
      --pool_size arg         Size of persistent pool (in Bytes) (default: 
                              0)
      --skip_load             Skip the load phase
      --latency_sampling arg  Sample latency of requests (default: 
                              0.000000)
  -m, --mode arg              Benchmark mode (default: operation)
      --seconds arg           Time (seconds) PiBench run in time-based mode 
                              (default: 20)
      --write_again           Write again
      --help                  Print help
```

# Tips
Because of the different characteristics of different key-value stores, it is difficult to have perfect compatibility and some problems may occur. So we give some existing problems and tips for different databases respectively.

## FlatStore
We use different versions of FlatStore with different indexes. Here are their correspondences：

|  Directory  | Index  | Description |
|  ----  | ----  | ---- |
| flatstore-pacman  | [Fast&Fair B+-tree](https://github.com/DICL/FAST_FAIR) | A persistent B+-tree |
|  flatstore-m | [Masstree](https://github.com/kohler/masstree-beta ) | A volatile trie where each node is a B+-tree. We consider it to be a B+-tree because with 8-byte keys it degrades to only one trie node. |
|flatstore-NB| [NBTree](https://github.com/SJTU-DDST/NBTree)| A lock-free PM-friendly persistent B+-Tree. For fairness, we disable its eADR-based optimization.
|flatstore-LB| [LB+-Tree](https://github.com/schencoding/lbtree)| A persistent B+-tree which uses HTM to optimize for concurrency.
|flatstore-DP| [DPTree](https://github.com/zxjcarrot/DPTree-code)| A multi-stage index including a volatile B+-tree stage, a read-only B+-tree stage, and a DRAM-PM hybrid trie stage.|

After FlatStore executes a multi-threaded test, it may crash due to memory reclamation issues. Anyway, this does not affect the display of performance results.

## ListDB
ListDB's datapath is fixed in the code, so we can't set the datapath via the parameter `pool_path`. The path set by the author is `/mnt/pmem/wkim/`.

## FluidKV
Target data path does not exist can cause a crash.

## pactree
Requires tbb dependencies. (`apt install libtbb-dev`)

## rocksdb
Intel `pmem-rocksdb` only supports only supports compilation using Makefile, so we write the `make` command into `CMakeList.txt`. This will result in a Make operation every time you compile and a small lag.


We'll be adding more details and tips as we go along...