#include "nng_exception.h"

namespace nng {

    nng_exception::nng_exception() : std::exception() {
        error_code = -1;
    }

    nng_exception::nng_exception(int ec) : std::exception() {
        message = ::nng_strerror(error_code = ec);
    }

    nng_exception::~nng_exception() {        
    }
}
