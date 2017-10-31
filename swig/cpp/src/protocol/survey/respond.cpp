#include "respond.h"

namespace nng {
    namespace protocol {
        namespace v0 {

            using std::placeholders::_1;

            // While we could use nng_respond_open, I think it is sufficient to just use the versioned symbol.
            respond_socket::respond_socket() : _Socket(std::bind(&(::nng_respondent0_open), _1)) {
            }

            respond_socket::~respond_socket() {
            }
        }
    }
}
