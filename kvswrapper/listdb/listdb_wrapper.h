#pragma once

#define LISTDB_STRING_KEY
#define LISTDB_WISCKEY

#include "kvs_api.hpp"
#include <string>
#include <vector>
#include <cassert>
#include "listdb/listdb.h"
#include "listdb/db_client.h"

#include <sstream>

#include <cstdint>
#include <iostream>
#include <type_traits>
#include <cstring>
#include <array>
#include <mutex>
#include <shared_mutex>

class listdb_client : public client_api
{
public:
    listdb_client(DBClient *c);
    listdb_client(ListDB *db, int tid);

    virtual ~listdb_client();

    virtual bool insert(const char *key, size_t key_sz, const char *value, size_t value_sz) override;
    virtual bool update(const char *key, size_t key_sz, const char *value, size_t value_sz) override;
    virtual bool remove(const char *key, size_t key_sz) override;

    virtual bool find(const char *key, size_t sz, char *value_out) override;
    virtual bool find(const char *key, size_t sz, std::string &value_out) override;

    virtual int scan(const char *key, size_t key_sz, int scan_sz, char *&values_out) override;

private:
    DBClient *client;
};

listdb_client::listdb_client(DBClient *c)
{
    client = c;
}
listdb_client::listdb_client(ListDB *db, int tid)
{
    client = new DBClient(db, tid, 0);
}

listdb_client::~listdb_client()
{
    delete client;
}

bool listdb_client::find(const char *key, size_t key_sz, char *value_out)
{
    size_t k = *(size_t *)key;
    size_t v;
    bool ret = client->Get(k, &v);
    if (ret)
    {
        v = *(PmemPtr::Decode<uint64_t>(v));
        return true;
    }
    return false;
}

bool listdb_client::find(const char *key, size_t key_sz, std::string &value_out)
{
    // uint64_t value_addr;
    // std::string_view k(key, key_sz);
    // bool ret = client->GetStringKV(k, &value_addr);
    // if (ret)
    // {
    //     // printf("success\n");
    //     char *p = (char *)value_addr;
    //     size_t val_len = *((uint64_t *)p);
    //     p += sizeof(size_t);
    //     value_out.assign(p, val_len);
    //     return true;
    // }

    size_t k = *(size_t *)key;
    size_t v;
    bool ret = client->Get(k, &v);
    if (ret)
    {
        size_t valueout = *(PmemPtr::Decode<uint64_t>(v));
        value_out.assign((char *)&valueout, 8);
        return true;
    }
    return false;
}

bool listdb_client::insert(const char *key, size_t key_sz, const char *value, size_t value_sz)
{
    // std::string_view k(key, 8);
    // std::string_view v(value, 8);
    // client->PutStringKV(k, v);
    size_t k = *(size_t *)key;
    size_t v = *(size_t *)value;
    client->Put(k, v);
    return true;
}

bool listdb_client::update(const char *key, size_t key_sz, const char *value, size_t value_sz)
{
    return insert(key, key_sz, value, value_sz);
}

bool listdb_client::remove(const char *key, size_t key_sz)
{
    return false;
}
int listdb_client::scan(const char *key, size_t key_sz, int scan_sz, char *&values_out)
{
    return 0;
}

class listdb_wrapper : public kvs_api
{
public:
    listdb_wrapper(const kvs_options_t &opt);
    virtual ~listdb_wrapper();
    virtual inline bool use_string_for_find() override { return false; };
    virtual std::unique_ptr<client_api> get_client(int tid) override;

private:
    ListDB *listdb_db;
    DBClient *clients[32];
};

listdb_wrapper::listdb_wrapper(const kvs_options_t &opt)
{
    listdb_db = new ListDB();
    if (fs::exists("/mnt/pmem/wkim/listdb"))
        listdb_db->Open();
    else
        listdb_db->Init();
    for (int tid = 0; tid < 32; tid++)
    {
        clients[tid] = new DBClient(listdb_db, tid, 0);
    }
}

listdb_wrapper::~listdb_wrapper()
{
    if (listdb_db != nullptr)
    {
        delete listdb_db;
    }
}
std::unique_ptr<client_api> listdb_wrapper::get_client(int tid)
{
    return std::make_unique<listdb_client>(listdb_db, tid);
}