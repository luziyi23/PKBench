#ifndef __LIBRARY_LOADER_HPP__
#define __LIBRARY_LOADER_HPP__

#include "kvs_api.hpp"

#include <string>

namespace PiBench
{

class library_loader_t
{
public:
    /**
     * @brief Construct a new library loader object
     *
     * @param path Absolute path of library file to be loaded.
     */
    library_loader_t(const std::string& path);

    /**
     * @brief Destroy the library loader object
     *
     */
    ~library_loader_t();

    /**
     * @brief Create a tree object
     *
     * Call create_kvs function implemented by the library.
     *
     * @param tree_opt workload options useful for optimizing tree layout.
     * @return kvs_api*
     */
    kvs_api* create_kvs(const kvs_options_t& tree_opt);

private:
    /// Handle for the dynamic library loaded.
    void* handle_;

    /// Pointer to factory function resposinble for instantiating a tree.
    kvs_api* (*create_fn_)(const kvs_options_t&);
};
} // namespace PiBench
#endif