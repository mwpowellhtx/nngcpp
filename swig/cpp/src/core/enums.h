#ifndef NNGCPP_ENUMS_H
#define NNGCPP_ENUMS_H

#define NNG_ONLY
#include <nngcpp.h>

#include <cstddef>

namespace nng {

    // Flags.
    enum flag_type {
        flag_none = 0,
        flag_alloc = ::NNG_FLAG_ALLOC,
        flag_nonblock = ::NNG_FLAG_NONBLOCK
    };

    enum duration_ms : int32_t {
        dur_zero = NNG_DURATION_ZERO,
        dur_infinite = NNG_DURATION_INFINITE,
        dur_default = NNG_DURATION_DEFAULT
    };

    enum protocol_type {
        protocol_none = ::NNG_PROTO_NONE,

        proto_bus_v0 = ::NNG_PROTO_BUS_V0,
        proto_pair_v0 = ::NNG_PROTO_PAIR_V0,
        proto_pair_v1 = ::NNG_PROTO_PAIR_V1,
        proto_subscriber_v0 = ::NNG_PROTO_SUB_V0,
        proto_publisher_v0 = ::NNG_PROTO_PUB_V0,
        proto_requestor_v0 = ::NNG_PROTO_REQ_V0,
        proto_replier_v0 = ::NNG_PROTO_REP_V0,
        proto_pusher_v0 = ::NNG_PROTO_PUSH_V0,
        proto_puller_v0 = ::NNG_PROTO_PULL_V0,
        proto_surveyor_v0 = ::NNG_PROTO_SURVEYOR_V0,
        proto_respondent_v0 = ::NNG_PROTO_RESPONDENT_V0,
        proto_star_v0 = ::NNG_PROTO_STAR_V0,

        proto_bus = ::NNG_PROTO_BUS,
        proto_pair = ::NNG_PROTO_PAIR,
        proto_subscriber = ::NNG_PROTO_SUB,
        proto_publisher = ::NNG_PROTO_PUB,
        proto_requestor = ::NNG_PROTO_REQ,
        proto_replier = ::NNG_PROTO_REP,
        proto_pusher = ::NNG_PROTO_PUSH,
        proto_puller = ::NNG_PROTO_PULL,
        proto_surveyor = ::NNG_PROTO_SURVEYOR,
        proto_respondent = ::NNG_PROTO_RESPONDENT,
        // TODO: TBD: NNG_PROTO_STAR undefined as of now; expecting NNG_PROTO_STAR moving forward? with protocol to boot?
        proto_star = ::NNG_PROTO_STAR_V0
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
