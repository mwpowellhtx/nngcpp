#include "endpoint.h"
#include "invocation.hpp"

namespace nng {

    _EndPoint::_EndPoint()
        : IHaveOne(), ICanClose(), IHaveOptions()
        , __start(), __close() {
    }

    _EndPoint::~_EndPoint() {
    }

    void _EndPoint::configure_endpoint(
        const start_func& start
        , const close_func& close) {

        const_cast<start_func&>(__start) = start;
        const_cast<close_func&>(__close) = close;
    }
}
