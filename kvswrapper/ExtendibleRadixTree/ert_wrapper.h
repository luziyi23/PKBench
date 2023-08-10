#pragma once

#include "kvs_api.hpp"
#include <atomic>
#include <iostream>
#include <unistd.h>
#include <map>
#include <sys/time.h>
#include "rng/rng.h"
#include "extendible_radix_tree/ERT_int.h"

class ert_client : public client_api
{
public:
    ert_client(ERTInt *db);

    virtual ~ert_client();

    virtual bool insert(const char *key, size_t key_sz, const char *value, size_t value_sz) override;
    virtual bool update(const char *key, size_t key_sz, const char *value, size_t value_sz) override;
    virtual bool remove(const char *key, size_t key_sz) override;

    virtual bool find(const char *key, size_t sz, char *value_out) override;
    virtual bool find(const char *key, size_t sz, std::string &value_out) override;

    virtual int scan(const char *key, size_t key_sz, int scan_sz, char *&values_out) override;

private:
    ERTInt* client;
};

ert_client::ert_client(ERTInt *db):client(db)
{
}
ert_client::~ert_client()
{
}

bool ert_client::find(const char *key, size_t key_sz, char *value_out)
{
    size_t v = client->Search(*(size_t *)key);
    *(value_out) = v;
    return true;
}

bool ert_client::find(const char *key, size_t key_sz, std::string &value_out)
{
    return false;
}

bool ert_client::insert(const char *key, size_t key_sz, const char *value, size_t value_sz)
{
    client->Insert(*(size_t*)key,*(size_t*)value);
    return true;
}

bool ert_client::update(const char *key, size_t key_sz, const char *value, size_t value_sz)
{
    return insert(key, key_sz, value, value_sz);
}

bool ert_client::remove(const char *key, size_t key_sz)
{
    return false;
}
int ert_client::scan(const char *key, size_t key_sz, int scan_sz, char *&values_out)
{
    return 0;
}

class ert_wrapper : public kvs_api
{
public:
    ert_wrapper(const kvs_options_t &opt);
    virtual ~ert_wrapper();
    virtual inline bool use_string_for_find() override { return false; };
    virtual std::unique_ptr<client_api> get_client(int tid = 0) override;
    virtual void print_stats() override{};
    virtual void wait() override{}

private:
    ERTInt *db;
};

ert_wrapper::ert_wrapper(const kvs_options_t &opt)
{
    init_fast_allocator(true);
    db = NewExtendibleRadixTreeInt();
}

ert_wrapper::~ert_wrapper()
{
    fast_free();
    if (db != nullptr)
    {
        delete db;
    }
}
std::unique_ptr<client_api> ert_wrapper::get_client(int tid)
{
    return std::make_unique<ert_client>(db);
}