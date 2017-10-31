#ifndef NNGCPP_ENUMS_H
#define NNGCPP_ENUMS_H

#define NNG_ONLY
#include <nngcpp.h>

#include "types.h"

#include <cstddef>

namespace nng {

    // Flags.
    enum flag_type : nng::int32_t {
        flag_none = 0,
        flag_alloc, // = ::NNG_FLAG_ALLOC,
        flag_nonblock, // = ::NNG_FLAG_NONBLOCK
    };

    enum duration_ms : int32_t {
        dur_zero = NNG_DURATION_ZERO,
        dur_infinite = NNG_DURATION_INFINITE,
        dur_default = NNG_DURATION_DEFAULT
    };

    /* We need to be very careful with these numbers owing to the underlying C API.
    On account of SWIG comprehension, we must be careful to define these precisely
    in agreement with the C code. Also leveraging the fact that Protocol is always
    a 16-bit unsigned field. */
    enum protocol_type : nng::uint16_t {
        protocol_none = 0, // ::NNG_PROTO_NONE,

        /* TODO: TBD: these ought to be a little more robust definitions especially
        approaching SWIG. If not, we will revisit at a later time. The C code multiplies
        by 16, but it's the same as shifting left. Plus C# can support that, for starters. */
        proto_pair_v0 = 1 << 4, // ::NNG_PROTO_PAIR_V0,
        proto_pair_v1 = 1 << 4 | 1, // ::NNG_PROTO_PAIR_V1,
        proto_publisher_v0 = 2 << 4, // ::NNG_PROTO_PUB_V0,
        proto_subscriber_v0 = 2 << 4 | 1, // ::NNG_PROTO_SUB_V0,
        proto_requestor_v0 = 3 << 4, // ::NNG_PROTO_REQ_V0,
        proto_replier_v0 = 3 << 4 | 1, // ::NNG_PROTO_REP_V0,
        proto_pusher_v0 = 5 << 4, // ::NNG_PROTO_PUSH_V0,
        proto_puller_v0 = 5 << 4 | 1, // ::NNG_PROTO_PULL_V0,
        proto_surveyor_v0 = 6 << 4, // ::NNG_PROTO_SURVEYOR_V0,
        proto_respondent_v0 = 6 << 4 | 1, // ::NNG_PROTO_RESPONDENT_V0,
        proto_bus_v0 = 7 << 4 | 0, // ::NNG_PROTO_BUS_V0,
        proto_star_v0 = 100 << 4, // ::NNG_PROTO_STAR_V0,

        /* Ditto robustness. Instead of depending on the C definitions, we will leverage our
        C++ definitons. SWIG should be able to rename these, but we will cross that bridge
        when we get there as well and as necessary. */ 
        proto_pair = proto_pair_v1,
        proto_publisher = proto_publisher_v0,
        proto_subscriber = proto_subscriber_v0,
        proto_requestor = proto_requestor_v0,
        proto_replier = proto_replier_v0,
        proto_pusher = proto_pusher_v0,
        proto_puller = proto_puller_v0,
        proto_surveyor = proto_surveyor_v0,
        proto_respondent = proto_respondent_v0,
        proto_bus = proto_bus_v0,
        // TODO: TBD: NNG_PROTO_STAR undefined as of now; expecting NNG_PROTO_STAR moving forward? with protocol to boot?
        proto_star = proto_star_v0
    };

    // https://www.gnu.org/software/libc/manual/html_node/Error-Codes.html
    enum error_code_type : uint32_t {
        ec_enone = 0,
        ec_eintr = ::NNG_EINTR,
        ec_enomem = ::NNG_ENOMEM,
        ec_einval = NNG_EINVAL,
        ec_ebusy = NNG_EBUSY,
        ec_etimedout = NNG_ETIMEDOUT,
        ec_econnrefused = NNG_ECONNREFUSED,
        ec_eclosed = NNG_ECLOSED,
        ec_eagain = NNG_EAGAIN,
        ec_enotsup = NNG_ENOTSUP,
        ec_eaddrinuse = NNG_EADDRINUSE,
        ec_estate = NNG_ESTATE,
        ec_enoent = NNG_ENOENT,
        ec_eproto = NNG_EPROTO,
        ec_eunreachable = NNG_EUNREACHABLE,
        ec_eaddrinval = NNG_EADDRINVAL,
        ec_eperm = NNG_EPERM,
        ec_emsgsize = NNG_EMSGSIZE,
        ec_econnaborted = NNG_ECONNABORTED,
        ec_econnreset = NNG_ECONNRESET,
        ec_ecanceled = NNG_ECANCELED,
        ec_enofiles = NNG_ENOFILES,
        ec_enospc = NNG_ENOSPC,
        ec_eexist = NNG_EEXIST,
        ec_ereadonly = NNG_EREADONLY,
        ec_ewriteonly = NNG_EWRITEONLY,
        ec_einternal = NNG_EINTERNAL,
        /* These are less of an enumeration and more of a mask... potentially.
        Especially these two, SYSERR and TRANERR. */
        ec_esyserr = NNG_ESYSERR,
        ec_etranerr = NNG_ETRANERR,
    };
}

#endif // NNGCPP_ENUMS_H
