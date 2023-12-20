#pragma once

#include "db.h"
#include "index/NBTree/nbtree.h"
extern uint64_t allocate_mem;
extern uint64_t allocate_size;

class NBTreeIndex : public Index {
 public:
  NBTreeIndex(IndexOptions ido) { 
	allocate_mem = ido.max_dram;
	size_t thread_dram_space = ido.max_dram / (ido.worker_thread_num) / (1024UL*1024UL*1024UL) * (1024UL*1024UL*1024UL);
	MEM_PER_THREAD=thread_dram_space;
  	MEM_OF_MAIN_THREAD=thread_dram_space;
	allocate_size = ido.max_pm;
    init_pmem_space(ido.pm_path.c_str());
	size_t thread_pm_space = ido.max_pm / (ido.worker_thread_num) / (1024UL*1024UL*1024UL) * (1024UL*1024UL*1024UL);
  	SPACE_PER_THREAD=thread_pm_space;
  	SPACE_OF_MAIN_THREAD=thread_pm_space;
	printf("pm space per thread: %lu GB\n",thread_pm_space / (1024UL*1024UL*1024UL));
    tree=new btree(); 
	for(size_t i=0;i<1000;i++){
		tree->insert(i,(char*)i);
	}
  }

  virtual ~NBTreeIndex() override { delete tree;}

  virtual ValueType Get(const Slice &key) override {
	ValueType value = (ValueType)tree->search(key.ToUint());
	// if(value==0){
	// 	printf("index nbtree get error (key=%016llx)\n",key.ToUint());
	// }
    return value & 0x3fffffffffffffffULL;
  }

  virtual void Put(const Slice &key, LogEntryHelper &le_helper) override {
	  assert(le_helper.new_val!=0);
	  tree->insert(key.ToUint(),(char*)le_helper.new_val);
  }

  virtual void GCMove(const Slice &key, LogEntryHelper &le_helper) override {

  }

  virtual void Delete(const Slice &key) override {
    tree->remove(key.ToUint());
  }

//   virtual void PrefetchEntry(const Shortcut &sc) override {
   
//   }

 private:
  btree* tree;

  DISALLOW_COPY_AND_ASSIGN(NBTreeIndex);
};
