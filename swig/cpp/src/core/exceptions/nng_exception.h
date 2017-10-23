#ifndef NNGCPP_NNG_EXCEPTION_H
#define NNGCPP_NNG_EXCEPTION_H

#define NNG_ONLY
#include <nngcpp.h>

// nng should be in the include path.
#include <exception>
#include <string>

namespace nng {

    // https://www.gnu.org/software/libc/manual/html_node/Error-Codes.html
    enum error_code_type {
        ec_eunknown = -1,
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
        ec_esyserr = NNG_ESYSERR,
        ec_etranerr = NNG_ETRANERR,
        // TODO: TBD: may also require exposing NNG_ETRANSPORT: https://github.com/nanomsg/nng/issues/128
        //ec_etransport = ::NNG_ETRANSPORT,
    };

    class nng_exception : public std::exception {
    public:

        error_code_type error_code;

    public:

        nng_exception();

        nng_exception(int errnum);

        nng_exception(error_code_type ec);

        virtual ~nng_exception();

        static const std::string strerror(int errnum);

        static const std::string strerror(error_code_type ec);

    private:

        template<typename ErrorCode_>
        static bool __one_of(ErrorCode_ ec) {
            // Not including itself.
            return false;
        }

        template<typename ErrorCode_, typename Arg_, typename... Args_>
        static bool __one_of(ErrorCode_ ec, const Arg_& arg, const Args_&... args) {
            // The variadics surrounding this are fairly generic. But this is where rubber meets asphalt.
            const auto actual_ec = static_cast<error_code_type>(ec);
            const auto actual_arg = static_cast<error_code_type>(arg);
            // True if ONE OF the args.
            return actual_ec == actual_arg || __one_of(ec, args...);
        }

    public:

        template<typename ErrorCode_, typename ...Args_>
        static bool one_of(ErrorCode_ ec, const Args_&... args) {
            return __one_of(ec, args...);
        }
    };
}

#define THROW_NNG_EXCEPTION_EC_IF_ONEOF(errnum, ...) if (nng::nng_exception::one_of(errnum, __VA_ARGS__)) throw nng::nng_exception(errnum)

#define THROW_NNG_EXCEPTION_IF_NOT_ONEOF(errnum, ...)  if (!nng::nng_exception::one_of(errnum, __VA_ARGS__)) throw nng::nng_exception(errnum)

// TODO: TBD: I don't know if -1 (unknown) is a possibility, but we will include it there for now.
#define THROW_NNG_EXCEPTION_EC(errnum) THROW_NNG_EXCEPTION_IF_NOT_ONEOF(errnum, nng::ec_enone)

#endif // NNGCPP_NNG_EXCEPTION_H
