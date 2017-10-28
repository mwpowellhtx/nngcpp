#include "endpoint.h"

namespace nng {

    endpoint::endpoint()
        : IHaveOne()
        , ICanClose()
        , ISupportOptions() {
    }

    endpoint::~endpoint() {
    }

    endpoint::options_type* const endpoint::GetOptions() {
        return support_options_type::GetOptions();
    }
}
