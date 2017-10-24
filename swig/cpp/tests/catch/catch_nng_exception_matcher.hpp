#ifndef CATCH_NNG_EXCEPTION_MATCHER_HPP
#define CATCH_NNG_EXCEPTION_MATCHER_HPP

#include <core/exceptions.hpp>

#include <ostream>

#include <catch.hpp>

namespace Catch {
    namespace Matchers {
        namespace Exceptions {

            // TODO: TBD: add/rename this one for "simple" ec exceptions
            // TODO: TBD: support expected system errors... also transport errors...

            //// TODO: TBD: methinks at this point that "matchers" are not what I'm thinking they are: i.e. not for "aligning with exceptions"
            //// TODO: TBD: Rather, that seems to be reserved for the REQUIRE_THROWS_WITH, which itself requires some comprehension of a string representation for my exceptions
            // TODO: TBD: this one is definitely potentially deserving of a dedicated header file, especially considering its broad utility
            struct NngExceptionMatcher : MatcherBase<nng::exceptions::nng_exception> {
            private:

                const nng::error_code_type _expected_ec;

                const nng::error_code_type _actual_ec;

                void set_actual_ec(nng::error_code_type ec) {
                    const_cast<nng::error_code_type&>(_actual_ec) = ec;
                }

            public:

                NngExceptionMatcher(const nng::error_code_type expected_ec)
                    : MatcherBase()
                    , _expected_ec(expected_ec)
                    , _actual_ec(nng::ec_enone) {
                }

                virtual bool match(const nng::exceptions::nng_exception& ex) const override {
                    // This may not be the safest thing to do here...
                    const_cast<NngExceptionMatcher*>(this)->set_actual_ec(ex.error_code);
                    return ex.error_code == _expected_ec;
                }

                virtual std::string describe() const override {
                    std::ostringstream os;
                    const auto errnum = static_cast<int>(_actual_ec);
                    os << "exception error code was: (" << errnum << ")";
                    return os.str();
                }
            };

            struct SystemExceptionMatcher : MatcherBase<nng::exceptions::system_error> {
            private:

                const uint32_t _expected_errnum;

                const uint32_t _actual_errnum;

                void set_actual_errnum(uint32_t errnum) {
                    const_cast<uint32_t&>(_actual_errnum) = errnum;
                }

            public:

                // TODO: TBD: this might actually be a separate exception altogether... system_exception...
                SystemExceptionMatcher(const nng::error_code_type expected_errnum)
                    : MatcherBase()
                    , _expected_errnum(expected_errnum)
                    , _actual_errnum(nng::ec_enone) {
                }

                virtual bool match(const nng::exceptions::system_error& ex) const override {
                    // This may not be the safest thing to do here...
                    const_cast<SystemExceptionMatcher*>(this)->set_actual_errnum(ex.error_number);
                    return ex.error_number == _expected_errnum;
                }

                virtual std::string describe() const override {
                    std::ostringstream os;
                    os << "exception error number was: (" << _actual_errnum << ")";
                    return os.str();
                }
            };

            struct TransportExceptionMatcher : MatcherBase<nng::exceptions::transport_error> {
            private:

                const uint32_t _expected_errnum;

                const uint32_t _actual_errnum;

                void set_actual_errnum(uint32_t errnum) {
                    const_cast<uint32_t&>(_actual_errnum) = errnum;
                }

            public:

                // TODO: TBD: this might actually be a separate exception altogether... system_exception...
                TransportExceptionMatcher(const nng::error_code_type expected_errnum)
                    : MatcherBase()
                    , _expected_errnum(expected_errnum)
                    , _actual_errnum(nng::ec_enone) {
                }

                virtual bool match(const nng::exceptions::transport_error& ex) const override {
                    // This may not be the safest thing to do here...
                    const_cast<TransportExceptionMatcher*>(this)->set_actual_errnum(ex.error_number);
                    return ex.error_number == _expected_errnum;
                }

                virtual std::string describe() const override {
                    std::ostringstream os;
                    os << "exception error number was: (" << _actual_errnum << ")";
                    return os.str();
                }
            };
        }
    }
}

#define THROWS_NNG_EXCEPTION(expected_ec) Catch::Matchers::Exceptions::NngExceptionMatcher(expected_ec)

#define THROWS_SYSTEM_EXCEPTION(expected_errnum) Catch::Matchers::Exceptions::SystemExceptionMatcher(expected_errnum)

#define THROWS_TRANSPORT_EXCEPTION(expected_errnum) Catch::Matchers::Exceptions::TransportExceptionMatcher(expected_errnum)

#endif // CATCH_NNG_EXCEPTION_MATCHER_HPP
