#include "pull.h"

namespace nng {
    namespace protocol {
        namespace v0 {

            using std::placeholders::_1;

            // While we could use nng_pull_open, I think it is sufficient to just use the versioned symbol.
            pull_socket::pull_socket() : socket(std::bind(&(::nng_pull0_open), _1)) {
            }

            pull_socket::~pull_socket() {
            }
        }
    }
}
