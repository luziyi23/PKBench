#include "pmemkv_wrapper.h"

extern "C" kvs_api* create_kvs(const kvs_options_t& opt) {
	return new pmemkv_wrapper(opt);
}

