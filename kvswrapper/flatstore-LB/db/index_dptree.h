#pragma once
#include "index/DPTree/include/btreeolc.hpp"
#include "index/DPTree/include/concur_dptree.hpp"
#include "db.h"

extern std::atomic<uint64_t> pm_allocated;
extern std::atomic<uint64_t> pm_freed;
extern int parallel_merge_worker_num;
size_t pool_size_ = ((size_t)(1024 * 1024 * 16) * 1024);
const char *pool_path_;
class DPTreeIndex : public Index
{
public:
	DPTreeIndex(IndexOptions ido)
	{
		parallel_merge_worker_num = ido.worker_thread_num;

		auto path_ptr = new std::string(ido.pm_path);
		if (*path_ptr != "")
			pool_path_ = (*path_ptr).c_str();
		else
			pool_path_ = "./pool";

		if (ido.max_pm != 0)
			pool_size_ = ido.max_pm;

		printf("PMEM Pool Path: %s\n", pool_path_);
		printf("PMEM Pool size: %ld\n", pool_size_);

		dptree = new dptree::concur_dptree<uint64_t, uint64_t>();
	}

	virtual ~DPTreeIndex() override {delete dptree;}

	virtual ValueType Get(const Slice &key) override
	{
		uint64_t v, k = key.ToUint();
		if (!dptree->lookup(k, v))
		{
			return false;
		}
		v = v >> 1;
		return v;
	}

	virtual void Put(const Slice &key, LogEntryHelper &le_helper) override
	{
		assert(le_helper.new_val != 0);
		uint64_t k = key.ToUint();
		uint64_t v = le_helper.new_val;
		dptree->insert(k, v);
	}

	virtual void GCMove(const Slice &key, LogEntryHelper &le_helper) override
	{
	}

	virtual void Delete(const Slice &key) override
	{
	}

	//   virtual void PrefetchEntry(const Shortcut &sc) override {

	//   }

	virtual void Scan(const Slice &key, int cnt,
					  std::vector<ValueType> &vec) override
	{
		uint64_t k = key.ToUint();
		static thread_local std::vector<uint64_t> v(cnt * 2);
		v.clear();
		dptree->scan(k, cnt, v);
		for(int i=0;i<v.size();i++){
			if(i%2!=0){
				vec.push_back(v[i]>>1);
			}
		}
	}

private:
	dptree::concur_dptree<uint64_t, uint64_t>* dptree;

	DISALLOW_COPY_AND_ASSIGN(DPTreeIndex);
};