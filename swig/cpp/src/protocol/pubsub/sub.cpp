#include "sub.h"

namespace nng {
    namespace protocol {
        namespace v0 {

            using std::placeholders::_1;

            // While we could use nng_sub_open, I think it is sufficient to just use the versioned symbol.
            sub_socket::sub_socket() : socket(std::bind(&(::nng_sub0_open), _1)) {
            }

            sub_socket::~sub_socket() {
            }
        }
    }
}
