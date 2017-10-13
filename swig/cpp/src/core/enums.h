#ifndef NNGCPP_ENUMS_H
#define NNGCPP_ENUMS_H

#include <nng/nng.h>

namespace nng {

    // Flags.
    enum flag_type {
        flag_none = 0,
        flag_alloc = ::NNG_FLAG_ALLOC,
        flag_nonblock = ::NNG_FLAG_NONBLOCK,
        flag_dryrun = NNG_FLAG_DRYRUN
    };
}

#endif // NNGCPP_ENUMS_H
