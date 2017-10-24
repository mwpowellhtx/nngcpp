#ifndef NNGCPP_CORE_EXCEPTIONS_HPP
#define NNGCPP_CORE_EXCEPTIONS_HPP

#include "exceptions/nng_exception.h"
#include "exceptions/not_implemented.h"
#include "exceptions/invalid_operation.h"
#include "exceptions/system_error.h"
#include "exceptions/transport_error.h"
#include "enums.h"

#include <vector>
#include <algorithm>

namespace trx {
    namespace exceptions {

        struct exception_utils {
        private:

            typedef nng::error_code_type error_code_type;

            typedef std::vector<nng::error_code_type> error_code_vector;

            exception_utils();

            template<typename ErrorCode_>
            static bool __one_of(ErrorCode_ ec) {
                // Not including itself.
                return false;
            }

            template<typename ErrorCode_, typename Arg_, typename... Args_>
            static bool __one_of(ErrorCode_ ec, const Arg_& arg, const Args_&... args) {
                // The variadics surrounding this are fairly generic. But this is where rubber meets asphalt.
                const auto given_ec = static_cast<error_code_type>(ec);
                const auto actual_arg = static_cast<error_code_type>(arg);
                /* This is also a bit tricky. First, if we expected the zero base case,
                then we expect neither of them. Otherwise, this is more of a MASK than
                a numeric EQUALITY. OR, whether ONE OF the other ones. */
                const auto _was_this_one = !(ec || arg) || given_ec & actual_arg;
                return _was_this_one || __one_of(ec, args...);
            }

            template<typename ErrorCode_, typename... Args_>
            static bool __one_of(ErrorCode_ ec, const error_code_vector& ecs, const Args_&... args) {
                // The variadics surrounding this are fairly generic. But this is where rubber meets asphalt.
                const auto given_ec = static_cast<error_code_type>(ec);
                const auto predicated_ec = [given_ec](const error_code_vector::value_type ec) {
                    return !(ec || given_ec) || given_ec & ec;
                };
                const auto _was_this_one = std::find_if(ecs.cbegin(), ecs.cend()
                    , predicated_ec) != ecs.cend();
                return _was_this_one || __one_of(ec, args...);
            }

        public:

            template<typename ErrorCode_, typename ...Args_>
            static bool one_of(ErrorCode_ ec, const Args_&... args) {
                return __one_of(ec, args...);
            }

            static std::string strerror(int32_t errnum);
        };
    }
}

#define STRERROR(x) trx::exceptions::exception_utils::strerror(x)

#define THROW_SYSTEM_ERROR_IF_NECESSARY(errnum) \
    if (trx::exceptions::exception_utils::one_of(errnum, nng::ec_esyserr)) \
        throw nng::exceptions::system_error(errnum)

#define THROW_TRANSPORT_ERROR_IF_NECESSARY(errnum) \
    if (trx::exceptions::exception_utils::one_of(errnum, nng::ec_etranerr)) \
        throw nng::exceptions::transport_error(errnum)

#define THROW_NNG_EXCEPTION_IF_ONE_OF_TYPE(errnum, type, ...) \
    THROW_SYSTEM_ERROR_IF_NECESSARY(errnum); \
    else THROW_TRANSPORT_ERROR_IF_NECESSARY(errnum); \
    else if (trx::exceptions::exception_utils::one_of(errnum, __VA_ARGS__)) throw type(errnum)

#define THROW_NNG_EXCEPTION_IF_NOT_ONE_OF_TYPE(errnum, type, ...) \
    THROW_SYSTEM_ERROR_IF_NECESSARY(errnum); \
    else THROW_TRANSPORT_ERROR_IF_NECESSARY(errnum); \
    else if (!trx::exceptions::exception_utils::one_of(errnum, __VA_ARGS__)) throw type(errnum)

#define THROW_NNG_EXCEPTION_EC_IF_ONE_OF(errnum, ...) \
    THROW_NNG_EXCEPTION_IF_ONE_OF_TYPE(errnum, nng::exceptions::nng_exception, __VA_ARGS__)

#define THROW_NNG_EXCEPTION_IF_NOT_ONE_OF(errnum, ...) \
    THROW_NNG_EXCEPTION_IF_NOT_ONE_OF_TYPE(errnum, nng::exceptions::nng_exception, __VA_ARGS__)

#define THROW_NNG_EXCEPTION_EC(errnum) \
    THROW_NNG_EXCEPTION_IF_NOT_ONE_OF(errnum, nng::ec_enone)

#endif // NNGCPP_CORE_EXCEPTIONS_HPP
