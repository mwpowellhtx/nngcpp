#ifndef NNGCPP_NNG_EXCEPTION_H
#define NNGCPP_NNG_EXCEPTION_H

#include "../nngcpp_integration.h"

// nng should be in the include path.
#include <exception>
#include <string>

namespace nng {

    // https://www.gnu.org/software/libc/manual/html_node/Error-Codes.html
    enum error_code_type {
        ec_eunknown = -1,
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
        ec_esyserr = NNG_ESYSERR,
        ec_etranerr = NNG_ETRANERR,
    };

    class nng_exception : public std::exception {
        public:

            error_code_type error_code;

            std::string message;

        public:

            nng_exception();

            nng_exception(int error_code);

            nng_exception(error_code_type error_code);

            virtual ~nng_exception();
    };
}

#define THROW_NNG_EXCEPTION() throw nng::nng_exception();
// TODO: TBD: I'm not sure that error codes are always "negative" in nature...
#define THROW_NNG_EXCEPTION_EC(ec) if (ec < 0) { throw nng:nng_exception(ec); }

#endif // NNGCPP_NNG_EXCEPTION_H
