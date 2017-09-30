#include "push.h"

namespace nng {
    namespace protocol {
        namespace v0 {

            using std::placeholders::_1;

            // While we could use nng_push_open, I think it is sufficient to just use the versioned symbol.
            push_socket::push_socket() : socket(std::bind(&(::nng_push0_open), _1)) {
            }

            push_socket::~push_socket() {
            }
        }
    }
}
