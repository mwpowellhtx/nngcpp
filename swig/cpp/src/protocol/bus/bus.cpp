#include "bus.h"

namespace nng {
    namespace protocol {
        namespace v0 {

            using std::placeholders::_1;

            // While we could use nng_bus_open, I think it is sufficient to just use the versioned symbol.
            bus_socket::bus_socket() : _Socket(std::bind(&(::nng_bus0_open), _1)) {
            }

            bus_socket::~bus_socket() {
            }
        }
    }
}
