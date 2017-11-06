#include "async_writer.h"
#include "../invocation.hpp"

namespace nng {

    _AsyncOptionWriter::_AsyncOptionWriter()
        : _setopt_duration() {
    }

    _AsyncOptionWriter::~_AsyncOptionWriter() {}

    void _AsyncOptionWriter::set_setters(
        const setopt_duration_func& setopt_duration) {

        // Cast the constness away from the setters long enough to install the new ones.
        const_cast<setopt_duration_func&>(_setopt_duration) = setopt_duration;
    }

    void _AsyncOptionWriter::SetTimeoutDuration(const duration_type& val) {
        SetTimeoutMilliseconds(val.count());
    }

    void _AsyncOptionWriter::SetTimeoutMilliseconds(duration_rep_type val) {
        invocation::with_void_return_value(_setopt_duration, val);
    }
}
