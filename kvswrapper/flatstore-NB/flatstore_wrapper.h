#pragma once

#include "kvs_api.hpp"
#include <string>
#include <vector>
#include <cassert>
#include "flatdb.h"
#include <sstream>

#include <cstdint>
#include <iostream>
#include <type_traits>
#include <cstring>
#include <array>
#include <mutex>
#include <shared_mutex>
#include <filesystem>

class flatstore_client : public client_api
{
public:
    flatstore_client(DB *db);

    virtual ~flatstore_client();

    virtual bool insert(const char *key, size_t key_sz, const char *value, size_t value_sz) override;
    virtual bool update(const char *key, size_t key_sz, const char *value, size_t value_sz) override;
    virtual bool remove(const char *key, size_t key_sz) override;

    virtual bool find(const char *key, size_t sz, char *value_out) override;
    virtual bool find(const char *key, size_t sz, std::string &value_out) override;

    virtual int scan(const char *key, size_t key_sz, int scan_sz, char *&values_out) override;

private:
    std::unique_ptr<DB::Worker> client;
};

flatstore_client::flatstore_client(DB *db)
{
    client = db->GetWorker();
}

flatstore_client::~flatstore_client()
{
	client.reset();
}

bool flatstore_client::find(const char *key, size_t key_sz, char *value_out)
{

    return true;
}

bool flatstore_client::find(const char *key, size_t key_sz, std::string &value_out)
{
    auto r = client->Get(Slice(key, key_sz), &value_out);
    return r;
}

bool flatstore_client::insert(const char *key, size_t key_sz, const char *value, size_t value_sz)
{
    client->Put(Slice(key, key_sz), Slice(value, value_sz));
    return true;
}

bool flatstore_client::update(const char *key, size_t key_sz, const char *value, size_t value_sz)
{
    return insert(key, key_sz, value, value_sz);
}

bool flatstore_client::remove(const char *key, size_t key_sz)
{
    auto r = client->Delete(Slice(key, key_sz));
    return r;
}
int flatstore_client::scan(const char *key, size_t key_sz, int scan_sz, char *&values_out)
{
    auto r = client->Scan(Slice(key), scan_sz);
    return r;
}

class flatstore_wrapper : public kvs_api 
{
public:
    flatstore_wrapper(const kvs_options_t &opt);
    virtual ~flatstore_wrapper();
    virtual inline bool use_string_for_find() override { return true; };
    virtual std::unique_ptr<client_api> get_client(int tid = 0) override;
    virtual void print_stats()override{
        flatstore_db->PrintStats();
    };
	virtual void wait(){
		sleep(10);
	};

private:
    DB *flatstore_db;
};

flatstore_wrapper::flatstore_wrapper(const kvs_options_t &opt)
{
    size_t log_size = opt.pool_size;
    if(std::filesystem::exists(opt.pool_path+"/log_pool"))
    {
        flatstore_db = new DB(opt.pool_path, log_size, opt.num_threads, 1);
#ifdef IDX_PERSISTENT
        flatstore_db->RecoverySegments();
        flatstore_db->RecoveryInfo();
#else
        flatstore_db->RecoveryAll();
#endif
    }else{
        flatstore_db = new DB(opt.pool_path, log_size, opt.num_threads, 1);
    }
    
    // flatstore_db->RecoveryAll();
}

flatstore_wrapper::~flatstore_wrapper()
{
    if(flatstore_db!=nullptr){
        // flatstore_db->StartCleanStatistics();
        delete flatstore_db;
    }
}
std::unique_ptr<client_api> flatstore_wrapper::get_client(int tid)
{
    return std::make_unique<flatstore_client>(flatstore_db);
}