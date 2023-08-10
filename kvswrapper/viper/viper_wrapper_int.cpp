#include "viper_wrapper_int.h"

extern "C" kvs_api* create_kvs(const kvs_options_t& opt) {
	return new viper_wrapper(opt);
}

