#include "dptree_wrapper.hpp"

int parallel_merge_worker_num = 1;
size_t key_size_ = 0;
size_t pool_size_ = ((size_t)(1024 * 1024 * 16) * 1024);
const char *pool_path_;

extern "C" kvs_api* create_kvs(const kvs_options_t& opt)
{
	parallel_merge_worker_num = opt.num_threads;

    auto path_ptr = new std::string(opt.pool_path);
    if (*path_ptr != "")
    	pool_path_ = (*path_ptr).c_str();
    else
		pool_path_ = "./pool";
	
    if (opt.pool_size != 0)
    	pool_size_ = opt.pool_size;

    printf("PMEM Pool Path: %s\n", pool_path_);
    printf("PMEM Pool size: %ld\n", pool_size_);

    return new dptree_wrapper();
}
