#ifndef NNGCPP_TYPES_H
#define NNGCPP_TYPES_H

#define NNG_ONLY
#include <nngcpp.h>

#include <cstddef>
#include <chrono>

namespace nng {

    // TODO: TBD: trying to contain the underlying type definitions/change a best we can as relates to testing the API
    typedef ::nng_duration duration_rep_type;

    typedef std::chrono::duration<duration_rep_type, std::milli> duration_type;

    typedef std::size_t size_type;
}

#endif // NNGCPP_TYPES_H
