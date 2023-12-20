#pragma once


#include "index/pactree/include/pactree.h"
#include "index/pactree/src/pactreeImpl.h"
#include "db.h"

extern std::atomic<uint64_t> pm_allocated;
extern std::atomic<uint64_t> pm_freed;
extern size_t pool_size_;
extern std::string *pool_dir_;
struct ThreadHelper
{
	pactree *tree;
	ThreadHelper(pactree *t)
	{
		tree = t;
		t->registerThread();
		// int id = omp_get_thread_num();
		// printf("Thread ID: %d\n", id);
	}
	~ThreadHelper() { tree->unregisterThread(); }
};
class PacTreeIndex : public Index
{
public:
	PacTreeIndex(IndexOptions ido)
	{
		auto path_ptr = new std::string(ido.pm_path);
		if (*path_ptr != "")
			pool_dir_ = path_ptr;
		else
			pool_dir_ = new std::string("./");

		if (ido.max_pm != 0)
			pool_size_ = ido.max_pm;

		printf("PMEM Pool Dir: %s\n", pool_dir_->c_str());
		printf("PMEM Pool size: %lu\n", pool_size_);

		tree_ = new pactree(1);
	}

	virtual ~PacTreeIndex() override
	{
		if (tree_ != nullptr)
			delete tree_;
	}

	virtual ValueType Get(const Slice &key) override
	{
		thread_local ThreadHelper t(tree_);
		Val_t value = tree_->lookup(key.ToUint());
		return value;
	}

	virtual void Put(const Slice &key, LogEntryHelper &le_helper) override
	{
		thread_local ThreadHelper t(tree_);
		tree_->insert(key.ToUint(), le_helper.new_val);
	}

	virtual void GCMove(const Slice &key, LogEntryHelper &le_helper) override
	{
	}

	virtual void Delete(const Slice &key) override
	{
		thread_local ThreadHelper t(tree_);
		tree_->remove(key.ToUint());
	}

	//   virtual void PrefetchEntry(const Shortcut &sc) override {

	//   }

	virtual void Scan(const Slice &key, int cnt,
					  std::vector<ValueType> &vec) override
	{
		thread_local ThreadHelper t(tree_);
		thread_local std::vector<uint64_t> v(cnt * 2);
		v.clear();
		int scanned = tree_->scan(key.ToUint(), (uint64_t)cnt, v);
		// vec=v;
		for(int i=0;i<v.size();i++){
			if(i%2!=0){
				vec.push_back(v[i]);
			}
		}
		

	}

private:
	pactree *tree_ = nullptr;

	DISALLOW_COPY_AND_ASSIGN(PacTreeIndex);
};