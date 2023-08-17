#pragma once
#include "kvs_api.hpp"
#include "include/db.h"
#include <string>
#include <vector>
#include <cassert>
#include <sstream>

#include <cstdint>
#include <iostream>
#include <type_traits>
#include <cstring>
#include <array>
#include <mutex>
#include <shared_mutex>
#include <filesystem>

class fluidkv_client : public client_api
{
public:
    fluidkv_client(DB *db);

    virtual ~fluidkv_client();

    virtual bool insert(const char *key, size_t key_sz, const char *value, size_t value_sz) override;
    virtual bool update(const char *key, size_t key_sz, const char *value, size_t value_sz) override;
    virtual bool remove(const char *key, size_t key_sz) override;

    virtual bool find(const char *key, size_t sz, char *value_out) override;
    virtual bool find(const char *key, size_t sz, std::string &value_out) override;

    virtual int scan(const char *key, size_t key_sz, int scan_sz, char *&values_out) override;

private:
    std::unique_ptr<DBClient> client;
    char vbuf[1024];
    Slice vout;
};

fluidkv_client::fluidkv_client(DB *db) : vout(vbuf)
{
    client = db->GetClient();
}
fluidkv_client::~fluidkv_client()
{
    client.reset();
}

bool fluidkv_client::find(const char *key, size_t key_sz, char *value_out)
{
    auto r = client->Get(Slice(key, key_sz), vout);
    return r;
}

bool fluidkv_client::find(const char *key, size_t key_sz, std::string &value_out)
{
    return false;
}

bool fluidkv_client::insert(const char *key, size_t key_sz, const char *value, size_t value_sz)
{
    client->Put(Slice(key, key_sz), Slice(value, value_sz));
    return true;
}

bool fluidkv_client::update(const char *key, size_t key_sz, const char *value, size_t value_sz)
{
    return insert(key, key_sz, value, value_sz);
}

bool fluidkv_client::remove(const char *key, size_t key_sz)
{
    auto r = client->Delete(Slice(key, key_sz));
    return r;
}
int fluidkv_client::scan(const char *key, size_t key_sz, int scan_sz, char *&values_out)
{
    std::vector<size_t> key_out;
    auto r = client->Scan(Slice(key, key_sz), scan_sz, key_out);
    return r;
}

class fluidkv_wrapper : public kvs_api
{
public:
    fluidkv_wrapper(const kvs_options_t &opt);
    virtual ~fluidkv_wrapper();
    virtual inline bool use_string_for_find() override { return false; };
    virtual std::unique_ptr<client_api> get_client(int tid = 0) override;
    virtual void print_stats() override
    {
        
    };
    virtual void wait() override
    {
        db->EnableReadOnlyMode();
        db->EnableReadOptimizedMode();
        db->WaitForFlushAndCompaction();
        db->DisableReadOnlyMode();
    }

private:
    DB *db;
};

fluidkv_wrapper::fluidkv_wrapper(const kvs_options_t &opt)
{
    size_t log_size = opt.pool_size;
    DBConfig cfg;
    cfg.pm_pool_path = opt.pool_path;
    cfg.pm_pool_size = opt.pool_size;
    if (std::filesystem::exists(opt.pool_path + "/manifest"))
    {
        cfg.recover = true;
    }
    db = new DB(cfg);
}

fluidkv_wrapper::~fluidkv_wrapper()
{
    if (db != nullptr)
    {
        delete db;
    }
}
std::unique_ptr<client_api> fluidkv_wrapper::get_client(int tid)
{
    return std::make_unique<fluidkv_client>(db);
}