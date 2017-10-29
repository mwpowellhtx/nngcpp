#ifndef NNGCPP_TYPES_H
#define NNGCPP_TYPES_H

#ifdef UNIT_TESTING
#include <nng.h>
#else // UNIT_TESTING
#include <nng/nng.h>
#endif // UNIT_TESTING

#include <cstddef>
#include <chrono>

namespace nng {

#ifdef USING_SWIG

    // Do this in order to workaround a SWIG concern not seeing the standard definition for ::uint32_t
    typedef int int32_t;

    typedef unsigned char uint8_t;
    typedef unsigned short uint16_t;
    typedef unsigned int uint32_t;
    typedef unsigned long long uint64_t;

    typedef unsigned long long size_type;

    typedef int duration_rep_type;

#else // USING_SWIG

    // TODO: TBD: unless perchance there is a way of telling SWIG what ALL of our include directories ought to be?
    typedef ::int32_t int32_t;

    typedef ::uint8_t uint8_t;
    typedef ::uint16_t uint16_t;
    typedef ::uint32_t uint32_t;
    typedef ::uint64_t uint64_t;

    typedef std::size_t size_type;

    // TODO: TBD: trying to contain the underlying type definitions/change a best we can as relates to testing the API
    typedef ::nng_duration duration_rep_type;

#endif // USING_SWIG

    typedef std::chrono::duration<duration_rep_type, std::milli> duration_type;

    typedef ::nng_msg msg_type;
}

#endif // NNGCPP_TYPES_H
