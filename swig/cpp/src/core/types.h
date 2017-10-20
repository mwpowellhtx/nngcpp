#ifndef NNGCPP_TYPES_H
#define NNGCPP_TYPES_H

#define NNG_ONLY
#include <nngcpp.h>

#include <cstddef>

namespace nng {

    typedef ::nng_duration duration_type;

    typedef std::size_t size_type;
}

#endif // NNGCPP_TYPES_H
