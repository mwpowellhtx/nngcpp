#include "endpoint.h"

namespace nng {

    endpoint::endpoint()
        : IHaveOne()
        , ICanClose()
        , supports_options() {
    }

    endpoint::~endpoint() {
    }
}
