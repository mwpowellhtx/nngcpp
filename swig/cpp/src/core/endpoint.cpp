#include "endpoint.h"

namespace nng {

    _EndPoint::_EndPoint()
        : IHaveOne()
        , ICanClose()
        , IHaveOptions() {
    }

    _EndPoint::~_EndPoint() {
    }

    _EndPoint::options_type* const _EndPoint::GetOptions() {
        return have_options_type::GetOptions();
    }
}
