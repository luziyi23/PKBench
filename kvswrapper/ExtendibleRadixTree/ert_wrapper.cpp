#include "ert_wrapper.h"

extern "C" kvs_api* create_kvs(const kvs_options_t& opt) {
	return new ert_wrapper(opt);
}

