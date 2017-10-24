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
        flag_nonblock = ::NNG_FLAG_NONBLOCK,
        flag_dryrun = NNG_FLAG_DRYRUN
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
