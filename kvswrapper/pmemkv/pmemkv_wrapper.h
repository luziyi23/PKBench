#pragma once

#include "kvs_api.hpp"
#include <string>
#include <vector>
#include <cassert>
#include <libpmemkv.hpp>
#include <sstream>

#include <cstdint>
#include <iostream>
#include <type_traits>
#include <cstring>
#include <array>
#include <mutex>
#include <shared_mutex>
#include <string_view>

class pmemkv_wrapper : public kvs_api
{
public:
    pmemkv_wrapper(const kvs_options_t &opt);

    virtual ~pmemkv_wrapper();

    virtual bool insert(const char *key, size_t key_sz, const char *value, size_t value_sz) override;
    virtual bool update(const char *key, size_t key_sz, const char *value, size_t value_sz) override;
    virtual bool remove(const char *key, size_t key_sz) override;

    virtual bool find(const char *key, size_t sz, char *value_out) override;
    virtual bool find(const char *key, size_t sz, std::string &value_out) override;
    virtual bool use_string_for_find() override { return true; };
    virtual int scan(const char *key, size_t key_sz, int scan_sz, char *&values_out) override;

private:
    std::string pool_file_;
    pmem::kv::db db_;
};

pmemkv_wrapper::pmemkv_wrapper(const kvs_options_t &opt)
{
    pool_file_ = opt.pool_path + "/pmemkv.pool";
    pmem::kv::config cfg;
    cfg.put_path(pool_file_);
    cfg.put_size(opt.pool_size);
    cfg.put_create_if_missing(true);
    pmem::kv::status s = db_.open("robinhood", std::move(cfg));
    if (s != pmem::kv::status::OK)
    {
        fprintf(stderr, "open error!%d\n",s);
    }
}

pmemkv_wrapper::~pmemkv_wrapper()
{
    db_.close();
}

bool pmemkv_wrapper::insert(const char *key, size_t key_sz, const char *value, size_t value_sz)
{
    std::string_view k(key, key_sz);
    std::string_view v(value, value_sz);
    pmem::kv::status s = db_.put(k, v);
    return s == pmem::kv::status::OK;
}

bool pmemkv_wrapper::update(const char *key, size_t key_sz, const char *value, size_t value_sz)
{
    return insert(key, key_sz, value, value_sz);
}

bool pmemkv_wrapper::remove(const char *key, size_t key_sz)
{
    std::string_view k(key, key_sz);
    pmem::kv::status s = db_.remove(k);
    return s == pmem::kv::status::OK;
}

bool pmemkv_wrapper::find(const char *key, size_t sz, char *value_out)
{
    return false;
}
bool pmemkv_wrapper::find(const char *key, size_t sz, std::string &value_out)
{
    std::string_view k(key, sz);
    pmem::kv::status s = db_.get(k, &value_out);
    return s == pmem::kv::status::OK;
}

int pmemkv_wrapper::scan(const char *key, size_t key_sz, int scan_sz, char *&values_out)
{
    static thread_local std::array<char, (1 << 20)> results;
    char *dst = results.data();
    auto iter = db_.new_read_iterator().get_value();
    pmem::kv::status s = iter.seek_higher_eq(std::string_view(key, key_sz));
    if (s != pmem::kv::status::OK)
    {
        return 0;
    }
    int found = 0;
    uint32_t offset = 0;
    for (size_t i = 0; i < scan_sz; i++)
    {
        found++;
        memcpy(dst, iter.key().get_value().data(), iter.key().get_value().size());
        dst += iter.key().get_value().size();
        memcpy(dst, iter.read_range().get_value().data(), iter.read_range().get_value().size());
        dst += iter.read_range().get_value().size();
        if (iter.next() != pmem::kv::status::OK)
            break;
    }
    return found;
}