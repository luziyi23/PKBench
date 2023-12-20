#pragma once

#include "kvs_api.hpp"
#include "pactree.h"
#include <numa-config.h>
#include "pactreeImpl.h"

#include <cstring>
#include <mutex>
#include <vector>
#include <shared_mutex>
#include <atomic>
#include <omp.h>

extern std::atomic<uint64_t> dram_allocated;
extern std::atomic<uint64_t> pmem_allocated;
extern std::atomic<uint64_t> dram_freed;
extern std::atomic<uint64_t> pmem_freed;

struct ThreadHelper
{
    pactree* tree;
    ThreadHelper(pactree *t)
    {
        tree=t;
        t->registerThread();
        // int id = omp_get_thread_num();
        // printf("Thread ID: %d\n", id);
    }
    ~ThreadHelper() {tree->unregisterThread();}
};
class pactree_client : public client_api
{
public:
    pactree_client(pactree *db) : tree_(db)
    {
        // tree_->registerThread();
    }
    virtual ~pactree_client()
    {
        // tree_->unregisterThread();
    }
    virtual bool find(const char *key, size_t key_sz, char *value_out) override;
    virtual bool insert(const char *key, size_t key_sz, const char *value, size_t value_sz) override;
    virtual bool update(const char *key, size_t key_sz, const char *value, size_t value_sz) override;
    virtual bool remove(const char *key, size_t key_sz) override;
    virtual int scan(const char *key, size_t key_sz, int scan_sz, char *&values_out) override;

private:
    pactree *tree_ = nullptr;
};

class pactree_wrapper : public kvs_api
{
public:
    pactree_wrapper();
    virtual ~pactree_wrapper();
    virtual inline bool use_string_for_find() override { return false; };
    virtual std::unique_ptr<client_api> get_client(int tid = 0) override
    {
        return std::make_unique<pactree_client>(tree_);
    }
    virtual void print_stats() override
    {
        printf("DRAM Allocated: %llu\n", dram_allocated.load());
        printf("DRAM Freed: %llu\n", dram_freed.load());
        printf("PMEM Allocated: %llu\n", pmem_allocated.load());
        printf("PMEM Freed: %llu\n", pmem_freed.load());
    };

private:
    pactree *tree_ = nullptr;
};

pactree_wrapper::pactree_wrapper()
{
    tree_ = new pactree(1);
}

pactree_wrapper::~pactree_wrapper()
{
#ifdef MEMORY_FOOTPRINT
    // printf("DRAM Allocated: %llu\n", dram_allocated.load());
    // printf("DRAM Freed: %llu\n", dram_freed.load());
    printf("PMEM Allocated: %llu\n", pmem_allocated.load());
    // printf("PMEM Freed: %llu\n", pmem_freed.load());
#endif
    if (tree_ != nullptr)
        delete tree_;
    // tree_ = nullptr;
}

bool pactree_client::find(const char *key, size_t key_sz, char *value_out)
{
    thread_local ThreadHelper t(tree_);
    Val_t value = tree_->lookup(*reinterpret_cast<Key_t *>(const_cast<char *>(key)));
    if (value == 0)
    {
        return false;
    }
    memcpy(value_out, &value, sizeof(value));
    return true;
}

bool pactree_client::insert(const char *key, size_t key_sz, const char *value, size_t value_sz)
{
    thread_local ThreadHelper t(tree_);
    if (!tree_->insert(*reinterpret_cast<Key_t *>(const_cast<char *>(key)), *reinterpret_cast<Val_t *>(const_cast<char *>(value))))
    {
        return false;
    }
    return true;
}

bool pactree_client::update(const char *key, size_t key_sz, const char *value, size_t value_sz)
{
    thread_local ThreadHelper t(tree_);
    if (!tree_->update(*reinterpret_cast<Key_t *>(const_cast<char *>(key)), *reinterpret_cast<Val_t *>(const_cast<char *>(value))))
    {
        return false;
    }
    return true;
}

bool pactree_client::remove(const char *key, size_t key_sz)
{
    thread_local ThreadHelper t(tree_);
    if (!tree_->remove(*reinterpret_cast<Key_t *>(const_cast<char *>(key))))
    {
        return false;
    }
    return true;
}

int pactree_client::scan(const char *key, size_t /*key_sz*/, int scan_sz, char *& /*values_out*/)
{
    thread_local ThreadHelper t(tree_);
    // constexpr size_t ONE_MB = 1ULL << 20;
    // static thread_local char results[ONE_MB];
    thread_local std::vector<Val_t> results;
    results.reserve(scan_sz);
    int scanned = tree_->scan(*reinterpret_cast<Key_t *>(const_cast<char *>(key)), (uint64_t)scan_sz, results);
    //    if (scanned < 100)
    //	printf("%d records scanned!\n", scanned);
    return scanned;
}
