#pragma once

#include "kvs_api.hpp"
#include <string>
#include <vector>
#include <cassert>
#include "leveldb/db.h"
#include <sstream>

#include <cstdint>
#include <iostream>
#include <type_traits>
#include <cstring>
#include <array>
#include <mutex>
#include <shared_mutex>

/* Describes all of the sstables that make up the db contents. */
#define LEVELDB_SSTTABLE_STATS "leveldb.sstables"
/* Statistics about the internal operation of the DB. */
#define LEVELDB_STAT "leveldb.stats"
/* Approximate number of bytes of memory in use by the DB. */
#define LEVELDB_MEMORY_USAGE "leveldb.approximate-memory-usage"
/* Number of files at level (append level as number). */
#define LEVELDB_NUM_FILES_AT_LEVEL "leveldb.num-files-at-level"

class leveldb_wrapper : public kvs_api
{
public:
    leveldb_wrapper(const kvs_options_t &opt);

    virtual ~leveldb_wrapper();

    virtual bool insert(const char *key, size_t key_sz, const char *value, size_t value_sz) override;
    virtual bool update(const char *key, size_t key_sz, const char *value, size_t value_sz) override;
    virtual bool remove(const char *key, size_t key_sz) override;

    virtual bool find(const char *key, size_t sz, char *value_out) override;
    virtual bool find(const char *key, size_t sz, std::string &value_out) override;
    virtual bool use_string_for_find() override { return true; };
    virtual int scan(const char *key, size_t key_sz, int scan_sz, char *&values_out) override;

    void print_stat(leveldb::DB *db_, bool print_sst = false);

private:
    leveldb::DB *db;
    leveldb::Options options;
};
leveldb_wrapper::leveldb_wrapper(const kvs_options_t &opt)
{

    options.create_if_missing = true;
    options.write_buffer_size = 64 * 1024L * 1024L;
    options.compression = leveldb::kNoCompression;

    leveldb::Status status = leveldb::DB::Open(options, opt.pool_path, &db);
    if (!status.ok())
    {
        fprintf(stderr, "open error: %s\n", status.ToString().c_str());
        exit(1);
    }
}

leveldb_wrapper::~leveldb_wrapper()
{
    print_stat(db);
    delete db;
}

bool leveldb_wrapper::find(const char *key, size_t key_sz, char *value_out)
{
    static thread_local std::string str;
    // uint64_t k = __builtin_bswap64(*reinterpret_cast<const uint64_t *>(key));
    // leveldb::Status s = db->Get(leveldb::ReadOptions(), reinterpret_cast<const char *>(&k), &str);
    leveldb::Status s = db->Get(leveldb::ReadOptions(), leveldb::Slice(key, key_sz), &str);

    if (s.ok())
    {
        std::vector<char> result(str.begin(), str.end());
        result.push_back('\0');
        value_out = &result[0];
        return true;
    }
    return false;
}

bool leveldb_wrapper::find(const char *key, size_t key_sz, std::string &value_out)
{
    leveldb::Status s = db->Get(leveldb::ReadOptions(), leveldb::Slice(key,key_sz),&value_out);

    return s.ok();
}

bool leveldb_wrapper::insert(const char *key, size_t key_sz, const char *value, size_t value_sz)
{
    // specially compatible for int keys, no use to string keys
    // uint64_t k = __builtin_bswap64(*reinterpret_cast<const uint64_t *>(key));
    // leveldb::Status s = db->Put(leveldb::WriteOptions(), reinterpret_cast<const char *>(&k), value);
    leveldb::Status s = db->Put(leveldb::WriteOptions(), leveldb::Slice(key, key_sz), leveldb::Slice(value, value_sz));
    if (s.ok())
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool leveldb_wrapper::update(const char *key, size_t key_sz, const char *value, size_t value_sz)
{
    return insert(key, key_sz, value, value_sz);
}

bool leveldb_wrapper::remove(const char *key, size_t key_sz)
{
    // uint64_t k = __builtin_bswap64(*reinterpret_cast<const uint64_t *>(key));
    //   leveldb::Status s = db->Delete(leveldb::WriteOptions(), reinterpret_cast<const char *>(&k));
    leveldb::Status s = db->Delete(leveldb::WriteOptions(), leveldb::Slice(key, key_sz));

    if (s.ok())
    {
        return true;
    }
    else
    {
        return false;
    }
}

// TODO(Ziyi): Create an iterator API to show scan performance without data copy overhead.
int leveldb_wrapper::scan(const char *key, size_t key_sz, int scan_sz, char *&values_out)
{
    leveldb::Iterator *iterator = db->NewIterator(leveldb::ReadOptions());

    int scanned = 0;
    // uint64_t k = __builtin_bswap64(*reinterpret_cast<const uint64_t *>(key));

    static thread_local std::array<char, (1 << 20)> results;
    char *dst = results.data();

    for (iterator->Seek(leveldb::Slice(key, key_sz)); (iterator->Valid() && scanned < scan_sz); iterator->Next())
    {

        leveldb::Slice result_key = iterator->key();
        memcpy(dst, result_key.data(), result_key.size());
        dst += sizeof(uint64_t);
        leveldb::Slice result_value = iterator->value();
        memcpy(dst, result_value.data(), result_value.size());
        ++scanned;
    }

    delete iterator;
    values_out = results.data();

    return scanned;
}

void leveldb_wrapper::print_stat(leveldb::DB *db_, bool print_sst)
{

    std::string stats_property;
    std::string sst_property;
    std::string mem_usage_property;
    std::string numfiles_level_property;

    std::cout << std::string(50, '=') << "\n";
    if (print_sst)
    {
        db_->GetProperty(LEVELDB_SSTTABLE_STATS, &sst_property);
        std::cout << "LevelDB SST Description:"
                  << "\n"
                  << sst_property << std::endl;
    }

    db_->GetProperty(LEVELDB_STAT, &stats_property);
    std::cout << "LevelDB Stats:"
              << "\n"
              << stats_property << std::endl;

    db_->GetProperty(LEVELDB_MEMORY_USAGE, &mem_usage_property);
    std::cout << "LevelDB Memory Usage:"
              << "\n\t"
              << mem_usage_property << " (B)" << std::endl;

    // print number of files at each levels
    std::cout << "LevelDB Number of files at each levels:" << std::endl;
    uint32_t level = 0;
    uint32_t total_files = 0;
    int num_files = 0;
    bool ret = false;

    while (true)
    {
        ret = db_->GetProperty(LEVELDB_NUM_FILES_AT_LEVEL + std::to_string(level), &numfiles_level_property);
        if (!ret)
        {
            break;
        }

        std::istringstream(numfiles_level_property) >> num_files;
        total_files += num_files;
        std::cout << "\tLevel " << level << ": " << num_files << std::endl;
        level++;
    }

    std::cout << "\tTotal: " << total_files << std::endl;
    std::cout << std::string(50, '=') << "\n";
}
