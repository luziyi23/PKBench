#include "library_loader.hpp"

#include <dlfcn.h>
#include <iostream>

namespace PiBench
{

library_loader_t::library_loader_t(const std::string& path)
{
    // Dynamically loads the library indicated by 'path'
    handle_ = dlopen(path.c_str(), RTLD_NOW);
    if (handle_ == nullptr)
    {
        std::cout << "Error in dlopen(): " << dlerror() << std::endl;
        exit(1);
    }

    // Search function 'create_kvs'
    dlerror();
    create_fn_ = (kvs_api * (*)(const kvs_options_t&)) dlsym(handle_, "create_kvs");
    auto err = dlerror();
    if (err != nullptr)
    {
        std::cout << "Error in dlsym(): " << err << std::endl;
        exit(1);
    }
    else if (create_fn_ == nullptr)
    {
        std::cout << "Could not find 'create()'" << std::endl;
        exit(1);
    }
}

library_loader_t::~library_loader_t()
{
    if (dlclose(handle_) != 0)
    {
        std::cout << "Error in dlclose()" << std::endl;
        return;
    }
}

kvs_api* library_loader_t::create_kvs(const kvs_options_t& opt)
{
    return create_fn_(opt);
}
} // namespace PiBench