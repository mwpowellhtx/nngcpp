#include "../exceptions.hpp"

namespace nng {
    namespace exceptions {

        transport_error::transport_error(int32_t errnum)
            : raw_number(errnum)
            , error_number(errnum&~ec_etranerr)
            , runtime_error(STRERROR(errnum)) {
        }

        transport_error::transport_error(int32_t errnum, const char* _Message)
            : raw_number(errnum)
            , error_number(errnum&~ec_etranerr)
            , runtime_error(_Message) {
        }

        transport_error::transport_error(int32_t errnum, const std::string& _Message)
            : raw_number(errnum)
            , error_number(errnum&~ec_etranerr)
            , runtime_error(_Message) {
        }

        transport_error::~transport_error() {
        }
    }
}
