#include "endpoint.h"

namespace nng {

    endpoint::endpoint()
        : having_one()
        , can_close()
        , supports_options() {
    }

    endpoint::~endpoint() {
    }
}
