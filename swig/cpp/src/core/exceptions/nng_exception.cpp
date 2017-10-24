#define NNG_ONLY
#include <nngcpp.h>

#include "../exceptions.hpp"

#include <string>
#include <functional>

// TODO: TBD: refactor this one to a better location? ...
namespace trx {
    namespace exceptions {

        const auto __strerror = std::bind(&::nng_strerror, std::placeholders::_1);

        std::string exception_utils::strerror(int32_t errnum) {
            return std::string(__strerror(errnum)).c_str();
        }
    }
}

namespace nng {
    namespace exceptions {

        nng_exception::nng_exception()
            : exception()
            , message()
            , error_code(ec_enone) {
        }

        nng_exception::nng_exception(uint32_t errnum)
            : exception()
            , message(STRERROR(errnum))
            , error_code(static_cast<error_code_type>(errnum)) {
        }

        nng_exception::nng_exception(error_code_type ec)
            : exception()
            , message(STRERROR(ec))
            , error_code(ec) {
        }

        nng_exception::~nng_exception() {
        }

        const char* nng_exception::what() const {
            return message.c_str();
        }
    }
}
