#include "endpoint.h"

namespace nng {

    endpoint::endpoint()
        : IHaveOne()
        , ICanClose()
        , IHaveOptions() {
    }

    endpoint::~endpoint() {
    }

    endpoint::options_type* const endpoint::GetOptions() {
        return have_options_type::GetOptions();
    }
}
