#include "nng_exception.h"

namespace nng {

    nng_exception::nng_exception() : exception() {
        error_code = ec_eunknown;
    }

    nng_exception::nng_exception(int errnum) : exception() {
        error_code = static_cast<error_code_type>(errnum);
        message = ::nng_strerror(errnum);
    }

    nng_exception::nng_exception(error_code_type ec) : exception() {
        error_code = ec;
        message = ::nng_strerror(static_cast<int>(ec));
    }

    nng_exception::~nng_exception() {        
    }

    const std::string nng_exception::strerror(int errnum) {
        const char* csp = ::nng_strerror(errnum);
        return std::string(csp);
    }

    const std::string nng_exception::strerror(error_code_type ec) {
        return strerror(static_cast<int>(ec));
    }

    char const* nng_exception::what() const {
        return "";
    }
}
