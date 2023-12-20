#ifndef __DPTREE_WRAPPER_HPP__
#define __DPTREE_WRAPPER_HPP__

#include "kvs_api.hpp"
#include <btreeolc.hpp>
#include <concur_dptree.hpp>

#include <cstring>
#include <mutex>
#include <shared_mutex>
#include <libpmemobj.h>
#include <atomic>

// #define DEBUG_MSG
std::atomic<uint64_t> pm_allocated(0);
std::atomic<uint64_t> pm_freed(0);
extern int parallel_merge_worker_num;

thread_local char k_[128];
thread_local uint64_t vkcmp_time = 0;
std::atomic<uint64_t> pmem_lookup;


class dptree_client: public client_api
{
public:
	dptree_client(dptree::concur_dptree<uint64_t, uint64_t>* dptree):tree_(dptree){

	}
	virtual ~dptree_client(){

	}
	virtual bool find(const char* key, size_t key_sz, char* value_out) override;
    virtual bool insert(const char* key, size_t key_sz, const char* value, size_t value_sz) override;
    virtual bool update(const char* key, size_t key_sz, const char* value, size_t value_sz) override;
    virtual bool remove(const char* key, size_t key_sz) override;
    virtual int scan(const char* key, size_t key_sz, int scan_sz, char*& values_out) override;

private:
	dptree::concur_dptree<uint64_t, uint64_t>* tree_;
};

class dptree_wrapper : public kvs_api
{
public:
    dptree_wrapper();
    virtual ~dptree_wrapper();
    virtual inline bool use_string_for_find() override { return false; };
	virtual std::unique_ptr<client_api> get_client(int tid = 0) override
    {
        return std::make_unique<dptree_client>(&dptree);
    }
	virtual void print_stats() override
    {
   		printf("PMEM Allocated: %lu, ", pm_allocated.load());
        printf("PMEM Freed: %lu, ", pm_freed.load());
		printf("PMEM Usage: %0.2lf GB\n", (double)(pm_allocated.load()-pm_freed.load())/1024/1024/1024);
    };
    

private:
    dptree::concur_dptree<uint64_t, uint64_t> dptree;
};

struct KV {
    uint64_t k;
    uint64_t v;
};

dptree_wrapper::dptree_wrapper()
{
}

dptree_wrapper::~dptree_wrapper()
{
}

bool dptree_client::find(const char* key, size_t key_sz, char* value_out)
{
    uint64_t v, k = *reinterpret_cast<uint64_t*>(const_cast<char*>(key));
    if (!tree_->lookup(k, v))
    {
        return false;
    }
    v = v >> 1;
    memcpy(value_out, &v, sizeof(v));
    return true;
}


bool dptree_client::insert(const char* key, size_t key_sz, const char* value, size_t value_sz)
{
    uint64_t k = *reinterpret_cast<uint64_t*>(const_cast<char*>(key));
    uint64_t v = *reinterpret_cast<uint64_t*>(const_cast<char*>(value));
    tree_->insert(k, v);
    return true;
}

bool dptree_client::update(const char* key, size_t key_sz, const char* value, size_t value_sz)
{
    uint64_t k = *reinterpret_cast<uint64_t*>(const_cast<char*>(key));
    uint64_t v = *reinterpret_cast<uint64_t*>(const_cast<char*>(value));
    tree_->upsert(k, v);
    return true;
}

bool dptree_client::remove(const char* key, size_t key_sz)
{
    // current dptree code does not implement delete
    return true;
}

int dptree_client::scan(const char* key, size_t key_sz, int scan_sz, char*& values_out)
{
    uint64_t k = *reinterpret_cast<uint64_t*>(const_cast<char*>(key));
    static thread_local std::vector<uint64_t> v(scan_sz*2);
    v.clear();
    tree_->scan(k, scan_sz, v);
    values_out = (char*)v.data();
    scan_sz = v.size() / 2;
    return scan_sz;
}

#endif
