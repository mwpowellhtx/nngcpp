#include "endpoint.h"

namespace nng {

    endpoint::endpoint()
        : IHaveOne()
        , ICanClose()
        , ISupportOptions() {
    }

    endpoint::~endpoint() {
    }
}
