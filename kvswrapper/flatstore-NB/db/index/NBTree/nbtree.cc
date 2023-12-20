#include "nbtree.h"
#include <unistd.h>
#include <sys/mman.h>

char *thread_space_start_addr;
__thread char *start_addr;
__thread char *curr_addr;
uint64_t allocate_size = 113ULL * 1024ULL * 1024ULL * 1024ULL;

char *thread_mem_start_addr;
__thread char *start_mem;
__thread char *curr_mem;
uint64_t allocate_mem = 113ULL * 1024ULL * 1024ULL * 1024ULL;


void init_pmem_space(const char* pm_path){
	// Create memory pool
	int fd = open(pm_path, O_RDWR | O_CREAT,0777);
	if (fd < 0)
	{
		printf("[NVM MGR]\tfailed to open nvm file\n");
		exit(-1);
	}
	if (ftruncate(fd, allocate_size) < 0)
	{
		printf("[NVM MGR]\tfailed to truncate file\n");
		exit(-1);
	}
	// void *pmem = mmap(NULL, allocate_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	// // memset(pmem, 0, SPACE_OF_MAIN_THREAD);
	// start_addr = (char *)pmem;
	// curr_addr = start_addr;
	// thread_space_start_addr = (char *)pmem + SPACE_OF_MAIN_THREAD;
	void *mem = new char[allocate_mem];
	start_mem = (char *)mem;
	curr_mem = start_mem;
	thread_mem_start_addr = (char *)mem + MEM_OF_MAIN_THREAD;
}