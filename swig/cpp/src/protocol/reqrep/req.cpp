#include "req.h"

namespace nng {
    namespace protocol {
        namespace v0 {

            using std::placeholders::_1;

            // While we could use nng_req_open, I think it is sufficient to just use the versioned symbol.
            req_socket::req_socket() : socket(std::bind(&(::nng_req0_open), _1)) {
            }

            req_socket::~req_socket() {
            }
        }
    }
}
