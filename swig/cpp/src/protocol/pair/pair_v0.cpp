#include "pair_v0.h"

namespace nng {
    namespace protocol {
        namespace v0 {

            using std::placeholders::_1;

            pair_socket::pair_socket() : socket(std::bind(&(::nng_pair0_open), _1)) {
            }

            pair_socket::~pair_socket() {
            }
        }
    }
}
