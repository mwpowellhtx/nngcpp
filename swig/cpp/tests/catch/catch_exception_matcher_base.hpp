//// TODO: TBD: this one does not really server much of a purposes for what it amounts to...
// #ifndef CATCH_EXCEPTION_MATCHER_BASE_HPP
// #define CATCH_EXCEPTION_MATCHER_BASE_HPP

// #include <catch.hpp>

// namespace Catch {
    // namespace Matchers {
        // namespace Exceptions {

            // //// TODO: TBD: methinks at this point that "matchers" are not what I'm thinking they are: i.e. not for "aligning with exceptions"
            // //// TODO: TBD: Rather, that seems to be reserved for the REQUIRE_THROWS_WITH, which itself requires some comprehension of a string representation for my exceptions
            // // TODO: TBD: this one is definitely potentially deserving of a dedicated header file, especially considering its broad utility
            // template <class Exception_>
            // struct ExceptionMatcherBase : MatcherBase<Exception_> {

            // protected:

               // ExceptionMatcherBase() :  MatcherBase() {
               // }
            // };
        // }

        // template <class Exception_, class ExceptionMatcher_ = ExceptionMatcherBase<Exception_> >
        // ExceptionMatcher_ ThrowsException() {
            // return Exceptions::ExceptionMatcher_();
        // }
    // }
// }

// #endif // CATCH_EXCEPTION_MATCHER_BASE_HPP
