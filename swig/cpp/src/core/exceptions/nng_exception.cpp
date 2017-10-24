#define NNG_ONLY
#include <nngcpp.h>

#include "../exceptions.hpp"

#include <functional>

// TODO: TBD: refactor this one to a better location? ...
namespace trx {
    namespace exceptions {

        const auto __strerror = std::bind(&::nng_strerror, std::placeholders::_1);

        const char* exception_utils::strerror(int32_t errnum) {
            return __strerror(errnum);
        }
    }
}

namespace nng {
    namespace exceptions {

        nng_exception::nng_exception()
            : exception()
            , error_code(ec_enone) {
        }

        nng_exception::nng_exception(uint32_t errnum)
            : exception(STRERROR(errnum))
            , error_code(static_cast<error_code_type>(errnum)) {
        }

        nng_exception::nng_exception(error_code_type ec)
            : exception(STRERROR(ec))
            , error_code(ec) {
        }

        nng_exception::~nng_exception() {
        }
    }
}
