#ifndef CATCH_NNG_EXCEPTION_MATCHER_HPP
#define CATCH_NNG_EXCEPTION_MATCHER_HPP

#include <catch.hpp>

#include <ostream>

#include <core/exceptions.hpp>

namespace Catch {
    namespace Matchers {
        namespace Exceptions {

            //// TODO: TBD: methinks at this point that "matchers" are not what I'm thinking they are: i.e. not for "aligning with exceptions"
            //// TODO: TBD: Rather, that seems to be reserved for the REQUIRE_THROWS_WITH, which itself requires some comprehension of a string representation for my exceptions
            // TODO: TBD: this one is definitely potentially deserving of a dedicated header file, especially considering its broad utility
            struct NngExceptionMatcher : MatcherBase<nng::nng_exception> {
            private:

                const nng::error_code_type _expected_ec;
                nng::error_code_type _actual_ec;

                void set_expected_ec(nng::error_code_type ec) {
                    _actual_ec = ec;
                }

            public:

            NngExceptionMatcher(const nng::error_code_type expected_ec)
                :  MatcherBase()
                , _expected_ec(expected_ec)
                , _actual_ec(nng::ec_eunknown) {
            }

                virtual bool match(const nng::nng_exception& ex) const override {
                    // This may not be the safest thing to do here...
                    const_cast<NngExceptionMatcher*>(this)->set_expected_ec(ex.error_code);
                    return ex.error_code == _expected_ec;
                }

                virtual std::string describe() const override {
                    std::ostringstream os;
                    const auto errnum = static_cast<int>(_actual_ec);
                    os << "exception error code was: (" << errnum << ")";
                    return os.str();
                }
            };
        }

        Exceptions::NngExceptionMatcher ThrowsNngException(const nng::error_code_type expected_ec) {
            return Exceptions::NngExceptionMatcher(expected_ec);
        }
    }
}

#endif // CATCH_NNG_EXCEPTION_MATCHER_HPP
