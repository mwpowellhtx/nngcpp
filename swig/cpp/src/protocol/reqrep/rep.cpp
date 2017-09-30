#include "rep.h"

namespace nng {
    namespace protocol {
        namespace v0 {

            using std::placeholders::_1;

            // While we could use nng_rep_open, I think it is sufficient to just use the versioned symbol.
            rep_socket::rep_socket() : socket(std::bind(&(::nng_rep0_open), _1)) {
            }

            rep_socket::~rep_socket() {
            }
        }
    }
}