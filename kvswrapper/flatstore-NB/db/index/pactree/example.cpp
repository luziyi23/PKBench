#include "pactree.h"
#define NUMDATA 10000

size_t pool_size_ = ((size_t)(1024 * 1024 * 16) * 1024);
std::string *pool_dir_;
int main()
{
    pool_size_ = ((size_t)(1024 * 1024 * 16) * 1024);
    auto path_ptr = new std::string("/mnt/pmem/ycsb");
    pool_dir_ = path_ptr;
    pactree* pt = new pactree(1);
    pt->registerThread();
    // for (size_t i = 1; i < NUMDATA; i++)
    // {
    //     pt->insert(i, i + 100);
    // }
    for (size_t i = 1; i < NUMDATA; i++)
    {
        if ((i + 100) != pt->lookup(i))
        {
            printf("error\n");
            exit(1);
        }
    }
    pt->unregisterThread();
    return 0;
}
