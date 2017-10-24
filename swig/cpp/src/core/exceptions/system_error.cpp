#include "../exceptions.hpp"

namespace nng {
    namespace exceptions {

        system_error::system_error(int32_t errnum)
            : raw_number(errnum)
            , error_number(errnum&~ec_esyserr)
            , runtime_error(STRERROR(errnum)) {
        }

        system_error::system_error(int32_t errnum, const char* _Message)
            : raw_number(errnum)
            , error_number(errnum&~ec_esyserr)
            , runtime_error(_Message) {
        }

        system_error::system_error(int32_t errnum, const std::string& _Message)
            : raw_number(errnum)
            , error_number(errnum&~ec_esyserr)
            , runtime_error(_Message) {
        }

        system_error::~system_error() {
        }
    }
}
