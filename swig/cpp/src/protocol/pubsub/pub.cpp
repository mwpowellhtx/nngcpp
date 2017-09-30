#include "pub.h"

namespace nng {
    namespace protocol {
        namespace v0 {

            using std::placeholders::_1;

            // While we could use nng_pub_open, I think it is sufficient to just use the versioned symbol.
            pub_socket::pub_socket() : socket(std::bind(&(::nng_pub0_open), _1)) {
            }

            pub_socket::~pub_socket() {
            }
        }
    }
}
