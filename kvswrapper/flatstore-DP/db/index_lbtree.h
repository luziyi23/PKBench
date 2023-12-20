#pragma once

#include "db.h"
#include "index/lbtree/lbtree-src/lbtree.h"
#define MASK 0x7fffffffffffffffUL;
inline key_type PBkeyToLB(const char *key, size_t size = 8)
{
    return (*reinterpret_cast<const key_type *>(key)) & MASK;
}

inline void *PBvalToLB(const char *value, size_t size = 8)
{
    return reinterpret_cast<void *>(*reinterpret_cast<const int64_t *>(value));
}

class LBTreeIndex : public Index {
 public:
  LBTreeIndex(IndexOptions ido) { 
	initUseful();
	worker_thread_num=ido.worker_thread_num+1;
	the_thread_mempools.init(worker_thread_num,ido.max_dram);
	printf("max_pm=%lu GB\n",ido.max_pm/1024/1024/1024);
	the_thread_nvmpools.init(worker_thread_num, ido.pm_path.c_str(),ido.max_pm);
	worker_id=0;

	char *nvm_addr= (char *)nvmpool_alloc(4*KB);
	the_treep= initTree(nvm_addr, false);
	nvmLogInit(worker_thread_num);

	simpleKeyInput input(2, 42069, 1);
	the_treep->bulkload(1, &input, 1.0);
}

  virtual ~LBTreeIndex() override { 
	the_thread_mempools.print_usage();
	the_thread_nvmpools.print_usage();
	delete the_treep; 
  }

  virtual ValueType Get(const Slice &key) override {
	void* p,*recptr;
	int pos;
    p=the_treep->lookup(PBkeyToLB(key.data()),&pos);
	if (pos<0){
		// printf("error: can't get %lu\n",key.ToUint());
		return INVALID_VALUE;
	}
	recptr = the_treep->get_recptr(p, pos);
    return (ValueType)recptr;
  }

  virtual void Put(const Slice &key, LogEntryHelper &le_helper) override {
	the_treep->insert(PBkeyToLB(key.data()),(void*)(le_helper.new_val));
  }

  virtual void GCMove(const Slice &key, LogEntryHelper &le_helper) override {
    the_treep->insert(PBkeyToLB(key.data()),(void*)(le_helper.new_val));
  }

  virtual void Delete(const Slice &key) override {
    the_treep->del(PBkeyToLB(key.data()));
  }

//   virtual void Scan(const Slice &key, int cnt,
//                     std::vector<ValueType> &vec) override {
//     char *results;
//     auto scanned = lbt_->scan(key.data(),8,cnt,results);
//     for(size_t i=scanned;i>0;i--){
//         vec.push_back(*(size_t *)results);
//         results+=2*sizeof(int);
//     }
//   }

  //virtual void PrefetchEntry(const Shortcut &sc) override {  }

 private:

  DISALLOW_COPY_AND_ASSIGN(LBTreeIndex);
};
