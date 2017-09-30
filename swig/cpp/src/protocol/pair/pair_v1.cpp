#include "pair_v1.h"

namespace nng {
    namespace protocol {
        namespace v1 {

            using std::placeholders::_1;

            pair_socket::pair_socket() : socket(std::bind(&(::nng_pair1_open), _1)) {
            }

            pair_socket::~pair_socket() {
            }
        }
    }
}
