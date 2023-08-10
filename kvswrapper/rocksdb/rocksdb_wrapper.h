#pragma once

#include "kvs_api.hpp"
#include <string>
#include <vector>
#include <cassert>
#define ON_DCPMM 1
#include "rocksdb/db.h"
#include <sstream>

#include <cstdint>
#include <iostream>
#include <type_traits>
#include <cstring>
#include <array>
#include <mutex>
#include <shared_mutex>
#include <thread>

class rocksdb_wrapper : public kvs_api
{
public:
    rocksdb_wrapper(const kvs_options_t &opt);
    virtual ~rocksdb_wrapper();
    virtual bool use_string_for_find() override { return true; };
    virtual std::unique_ptr<client_api> get_client(int id = 0);
    virtual void wait() override;

private:
    rocksdb::DB *db;
    rocksdb::Options options;
};

class rocksdb_client : public client_api
{
public:
    rocksdb_client(rocksdb::DB *_db) : db(_db){};

    virtual ~rocksdb_client(){};

    virtual bool insert(const char *key, size_t key_sz, const char *value, size_t value_sz) override;
    virtual bool update(const char *key, size_t key_sz, const char *value, size_t value_sz) override;
    virtual bool remove(const char *key, size_t key_sz) override;

    virtual bool find(const char *key, size_t sz, char *value_out) override;
    virtual bool find(const char *key, size_t sz, std::string &value_out) override;

    virtual int scan(const char *key, size_t key_sz, int scan_sz, char *&values_out) override;

private:
    rocksdb::DB *db;
    rocksdb::ReadOptions roptions;
    rocksdb::WriteOptions woptions;
};

rocksdb_wrapper::rocksdb_wrapper(const kvs_options_t &opt)
{
    options.create_if_missing = true;
    options.compression = rocksdb::kNoCompression;
    options.env = rocksdb::NewDCPMMEnv(rocksdb::DCPMMEnvOptions());
    options.recycle_dcpmm_sst = true;
    printf("open start\n");
    rocksdb::Status s = rocksdb::DB::Open(options, opt.pool_path, &db);
    if (!s.ok())
    {
        fprintf(stderr, "open error: %s\n", s.ToString().c_str());
        exit(-1);
    }
    printf("open over\n");
}

rocksdb_wrapper::~rocksdb_wrapper()
{
    db->Close();
    delete db;
}
void rocksdb_wrapper::wait(){
    std::this_thread::sleep_for(std::chrono::seconds(100));
}
std::unique_ptr<client_api> rocksdb_wrapper::get_client(int tid)
{
    return std::make_unique<rocksdb_client>(db);
}

bool rocksdb_client::find(const char *key, size_t key_sz, char *value_out)
{
    static thread_local std::string str;
    rocksdb::Status s = db->Get(roptions, rocksdb::Slice(key, key_sz), &str);

    if (s.ok())
    {
        std::vector<char> result(str.begin(), str.end());
        result.push_back('\0');
        value_out = &result[0];
        return true;
    }

    return false;
}

bool rocksdb_client::find(const char *key, size_t key_sz, std::string &value_out)
{
    rocksdb::Status s = db->Get(roptions, rocksdb::Slice(key, key_sz), &value_out);

    return s.ok();
}

bool rocksdb_client::insert(const char *key, size_t key_sz, const char *value, size_t value_sz)
{
    rocksdb::Status s = db->Put(woptions, rocksdb::Slice(key, key_sz), rocksdb::Slice(value, value_sz));
    return s.ok();
}

bool rocksdb_client::update(const char *key, size_t key_sz, const char *value, size_t value_sz)
{
    return insert(key, key_sz, value, value_sz);
}

bool rocksdb_client::remove(const char *key, size_t key_sz)
{
    rocksdb::Status s = db->Delete(woptions, rocksdb::Slice(key, key_sz));

    return s.ok();
}

// TODO(Ziyi): Create an iterator API to show scan performance without data copy overhead.
int rocksdb_client::scan(const char *key, size_t key_sz, int scan_sz, char *&values_out)
{
    rocksdb::Iterator *iterator = db->NewIterator(roptions);

    int scanned = 0;
    // uint64_t k = __builtin_bswap64(*reinterpret_cast<const uint64_t *>(key));

    static thread_local std::array<char, (1 << 20)> results;
    char *dst = results.data();
    for (iterator->Seek(rocksdb::Slice(key, key_sz)); (iterator->Valid() && scanned < scan_sz); iterator->Next())
    {
        rocksdb::Slice result_key = iterator->key();
        memcpy(dst, result_key.data(), result_key.size());
        dst += sizeof(uint64_t);
        rocksdb::Slice result_value = iterator->value();
        memcpy(dst, result_value.data(), result_value.size());
        ++scanned;
    }

    delete iterator;
    values_out = results.data();

    return scanned;
}