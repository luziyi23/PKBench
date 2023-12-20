#include "flatdb.h"
#include "util/kgen.h"
#include "util/stopwatch.hpp"
#include <iostream>
#include <thread>
#include <unistd.h>
#include <array>
#include <memory>
#include <string>
#include <stdexcept>
#include <regex>
#include <thread>

void print_dram_consuption()
{
    auto pid = getpid();
    std::array<char, 128> buffer;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(
        popen(("cat /proc/" + std::to_string(pid) + "/status").c_str(), "r"),
        pclose);
    if (!pipe)
    {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    {
        std::string result = buffer.data();
        if (result.find("VmRSS") != std::string::npos)
        {
            // std::cout << result << std::endl;
            std::string mem_ocp = std::regex_replace(
                result, std::regex("[^0-9]*([0-9]+).*"), std::string("$1"));
            printf("DRAM consumption: %.3f GB.\n", stof(mem_ocp) / 1024 / 1024);
            break;
        }
    }
}
// char value[101] = "valuexxxxxx";
const size_t thread_num = 16;
const size_t value_size = 8;
// Slice v(value, value_size);
void client_thread_put(DB *db, size_t start, size_t count)
{
    char keybuf[8];
    Slice k(keybuf, 8);
	char value[value_size+1];
	Slice v(value,value_size);
    std::unique_ptr<DB::Worker> c = db->GetWorker();
	size_t cnt=0;
    for (size_t i = start; i < start + count; i++)
    {
        size_t key = utils::multiplicative_hash<uint64_t>(i + 1);
        *(size_t *)keybuf = __builtin_bswap64(key);
		*(size_t *)value = i;
        // keybuf = __builtin_bswap64(i);
        // keybuf = __builtin_bswap64(keygen.next_id());
        // keybuf = i;
        c->Put(k, v);
		cnt++;
		if(cnt % 10000000==0)printf("thread %lu: %lu finished\n",start/count+1,cnt);
    }
}
void client_thread_get(DB *db, int tid, size_t start, size_t count)
{
    char keybuf[8];
    Slice k(keybuf, 8);
    std::unique_ptr<DB::Worker> c = db->GetWorker();
    std::string valueout;
	int cc=0;
	size_t cnt=0;
    for (size_t i = start; i < start + count; i++)
    {
        size_t key = utils::multiplicative_hash<uint64_t>(i + 1);
        *(size_t *)keybuf = __builtin_bswap64(key);
        // keybuf = __builtin_bswap64(i);
        // keybuf = __builtin_bswap64(keygen.next_id());
        // keybuf = i;
        auto success = c->Get(k, &valueout);
		// if (!success){
		// 	ERROR_EXIT("read error, %lu", i+1);
		// }
        if (!success || (*(size_t*)valueout.data()!=i) )
        {
            // LOG("read error, %lu,%lu", i+1,*(size_t*)valueout.data());
			cc++;
        }
		cnt++;
		if(cnt % 10000000==0)printf("thread %lu: %lu finished\n",start/count+1,cnt);
    }
	printf("error count:%d\n",cc);
}

int main()
{
    size_t workloads = 2000000;
    // size_t workloads = 5000000;
    std::string pm_pool_path = "/mnt/pmem/flatstore";
    DB *db = new DB(pm_pool_path, 10UL << 30, thread_num, 1);
    printf("getclient\n");
    uniform_key_generator_t keygen(workloads, 8);
    stopwatch_t sw;
    std::vector<std::thread> tlist, tlist2;
    sw.start();
    for (int i = 0; i < thread_num; i++)
    {
        tlist.emplace_back(std::thread(client_thread_put, db, workloads / thread_num * i, workloads / thread_num));
    }
    for (auto &th : tlist)
    {
        th.join();
    }
    auto us = sw.elapsed<std::chrono::microseconds>();
    std::cout << "count=" << workloads << ",write latency=" << us / workloads << " us,thpt=" << workloads * 1000000UL / us << "time:" << us << std::endl;
    print_dram_consuption();
    sw.clear();
    sleep(1);
    printf("get start2\n");
	std::unique_ptr<DB::Worker> c = db->GetWorker();
	char keybuf[8];
    Slice k(keybuf, 8);
	*(size_t *)keybuf = __builtin_bswap64(1);
	c->Scan(k,10);
	c.reset();
    int count = 0;
    sw.start();
    for (int i = 0; i < thread_num; i++)
    {
        tlist2.emplace_back(std::thread(client_thread_get, db, i + 1, workloads / thread_num * i, workloads / thread_num));
		tlist2.emplace_back(std::thread(client_thread_put, db, workloads + workloads / thread_num * i, workloads / thread_num));
    }
    printf("init over\n");
    fflush(stdout);
    for (auto &th : tlist2)
    {
        th.join();
    }
    us = sw.elapsed<std::chrono::microseconds>();
    std::cout << "count=" << workloads << ",read latency=" << us / workloads << " us,thpt=" << workloads * 1000000UL / us << "time:" << us << std::endl;
    fflush(stdout);
    printf("over\n");
    delete db;
    printf("close\n");

    return 0;
}