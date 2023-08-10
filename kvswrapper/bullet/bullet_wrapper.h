#pragma once

#include "kvs_api.hpp"
#include <string>
#include <vector>
#include <cassert>
#include "crl.hpp"
#include <sstream>

#include <cstdint>
#include <iostream>
#include <type_traits>
#include <cstring>
#include <array>
#include <mutex>
#include <shared_mutex>

class bullet_wrapper : public kvs_api
{
public:
    bullet_wrapper(const kvs_options_t &opt);

    virtual ~bullet_wrapper();

    virtual bool insert(const char *key, size_t key_sz, const char *value, size_t value_sz) override;
    virtual bool update(const char *key, size_t key_sz, const char *value, size_t value_sz) override;
    virtual bool remove(const char *key, size_t key_sz) override;

    virtual bool find(const char *key, size_t sz, char *&value_out) override;
    virtual bool find(const char *key, size_t sz, std::string &value_out) override;
    virtual bool use_string_for_find() override { return true; };
    virtual int scan(const char *key, size_t key_sz, int scan_sz, char *&values_out) override;

private:
    std::unique_ptr<bullet::CrlStore<std::string,std::string>::Client> client;
    std::unique_ptr<bullet::CrlStore<std::string,std::string>> bullet_db;
};


bullet_wrapper::bullet_wrapper(const kvs_options_t &opt)
{
    const size_t inital_size = opt.pool_size;
    std::string pool_path = opt.pool_path + "/bullet.front";
    std::string backend_path = opt.pool_path + "/bullet.back";
    bullet_db = std::make_unique<bullet::CrlStore<std::string,std::string>>(pool_path,backend_path,inital_size);
    client = std::make_unique<bullet::CrlStore<std::string,std::string>::Client>(bullet_db->get_client());
}

bullet_wrapper::~bullet_wrapper()
{
    if(client!=nullptr){
        client.reset();
    }
}

bool bullet_wrapper::find(const char *key, size_t key_sz, char *&value_out)
{
    return false;
}

bool bullet_wrapper::find(const char *key, size_t key_sz, std::string &value_out)
{
    auto r = client->get(std::string(key,key_sz),&value_out);
    return r;
}

bool bullet_wrapper::insert(const char *key, size_t key_sz, const char *value, size_t value_sz)
{
    auto r = client->put(std::string(key,key_sz),std::string(value,value_sz));
    return r;
}

bool bullet_wrapper::update(const char *key, size_t key_sz, const char *value, size_t value_sz)
{
    auto r = client->put(std::string(key,key_sz),std::string(value,value_sz));
    return r;
}

bool bullet_wrapper::remove(const char *key, size_t key_sz)
{
    auto r = client->remove(std::string(key,key_sz));
    return r;
}


int bullet_wrapper::scan(const char *key, size_t key_sz, int scan_sz, char *&values_out)
{
    return 0;
}