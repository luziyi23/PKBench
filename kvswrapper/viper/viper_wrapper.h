#pragma once

#include "kvs_api.hpp"
#include <string>
#include <vector>
#include <cassert>
#include "viper.hpp"
#include <sstream>

#include <cstdint>
#include <iostream>
#include <type_traits>
#include <cstring>
#include <array>
#include <mutex>
#include <shared_mutex>
class viper_wrapper : public kvs_api
{
public:
    viper_wrapper(const kvs_options_t &opt);
    ~viper_wrapper(){};
    virtual bool use_string_for_find() override { return true; };
    virtual std::unique_ptr<client_api> get_client(int tid) override;

private:
    std::unique_ptr<viper::Viper<std::string, std::string>> viper_db;
};
class viper_client : public client_api
{
public:
    viper_client(viper::Viper<std::string, std::string> *db);

    virtual ~viper_client();

    virtual bool insert(const char *key, size_t key_sz, const char *value, size_t value_sz) override;
    virtual bool update(const char *key, size_t key_sz, const char *value, size_t value_sz) override;
    virtual bool remove(const char *key, size_t key_sz) override;

    virtual bool find(const char *key, size_t sz, char *value_out) override;
    virtual bool find(const char *key, size_t sz, std::string &value_out) override;

    virtual int scan(const char *key, size_t key_sz, int scan_sz, char *&values_out) override;

private:
    std::unique_ptr<viper::Viper<std::string, std::string>::Client> client;
};

viper_client::viper_client(viper::Viper<std::string, std::string> *db)
{
    client = std::make_unique<viper::Viper<std::string, std::string>::Client>(db->get_client());
}

viper_client::~viper_client()
{
}

bool viper_client::find(const char *key, size_t key_sz, char *value_out)
{
    return false;
}

bool viper_client::find(const char *key, size_t key_sz, std::string &value_out)
{
    auto r = client->get(std::string(key, key_sz), &value_out);
    return r;
}

bool viper_client::insert(const char *key, size_t key_sz, const char *value, size_t value_sz)
{
    auto r = client->put(std::string(key, key_sz), std::string(value, value_sz));
    return r;
}

bool viper_client::update(const char *key, size_t key_sz, const char *value, size_t value_sz)
{
    auto r = client->put(std::string(key, key_sz), std::string(value, value_sz));
    return r;
}

bool viper_client::remove(const char *key, size_t key_sz)
{
    auto r = client->remove(std::string(key, key_sz));
    return r;
}

int viper_client::scan(const char *key, size_t key_sz, int scan_sz, char *&values_out)
{
    return 0;
}

viper_wrapper::viper_wrapper(const kvs_options_t &opt)
{

    const size_t inital_size = opt.pool_size;
    std::string pool_path = opt.pool_path;
    std::cout << pool_path << "," << inital_size << std::endl;
    if (!std::filesystem::exists(pool_path))
    {
        viper_db = viper::Viper<std::string, std::string>::create(pool_path,
                                                                  inital_size);
    }
    else
        viper_db = viper::Viper<std::string, std::string>::open(pool_path);
}

std::unique_ptr<client_api> viper_wrapper::get_client(int tid)
{
    return std::make_unique<viper_client>(viper_db.get());
}
