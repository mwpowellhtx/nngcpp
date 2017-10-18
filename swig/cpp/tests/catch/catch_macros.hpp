#ifndef CATCH_MACROS_HPP
#define CATCH_MACROS_HPP

#include <catch.hpp>

// Be careful of this include.
#define NNG_ONLY
#include <nngcpp.h>

#include <chrono>
#include <thread>

#ifndef INTERNAL_CATCH_THROWS_AS_MATCHING
///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_THROWS_AS_MATCHING( macroName, exceptionType, exceptionMatcher, resultDisposition, expr ) \
    do { \
        Catch::ResultBuilder __catchResult( macroName, CATCH_INTERNAL_LINEINFO, CATCH_INTERNAL_STRINGIFY(expr) ", " CATCH_INTERNAL_STRINGIFY(exceptionType), resultDisposition ); \
        if( __catchResult.allowThrows() ) \
            try { \
                static_cast<void>(expr); \
                __catchResult.captureResult( Catch::ResultWas::DidntThrowException ); \
            } \
            catch( const exceptionType& ex ) { \
                if ( exceptionMatcher.match( ex ) ) { \
                    __catchResult.captureResult( Catch::ResultWas::Ok ); \
                } \
                else { \
                    /* TODO: TBD: report not only that Did Not Throw Exception, but may also report Matcher Describe result. */ \
                    __catchResult.useActiveException( resultDisposition ); \
                } \
            } \
            catch( ... ) { \
                __catchResult.useActiveException( resultDisposition ); \
            } \
        else \
            __catchResult.captureResult( Catch::ResultWas::Ok ); \
        INTERNAL_CATCH_REACT( __catchResult ) \
    } while( Catch::alwaysFalse() )
#endif // INTERNAL_CATCH_THROWS_AS_MATCHING

#ifndef CATCH_CHECK_THROWS_AS_MATCHING
#define CATCH_CHECK_THROWS_AS_MATCHING( expr, exceptionType, exceptionMatcher ) INTERNAL_CATCH_THROWS_AS_MATCHING( "CATCH_CHECK_THROWS_AS_MATCHING", exceptionType, exceptionMatcher, Catch::ResultDisposition::ContinueOnFailure, expr )
#endif // CATCH_CHECK_THROWS_AS_MATCHING

#ifndef REQUIRE_THROWS_AS_MATCHING
#define REQUIRE_THROWS_AS_MATCHING( expr, exceptionType, exceptionMatcher ) INTERNAL_CATCH_THROWS_AS_MATCHING( "REQUIRE_THROWS_AS_MATCHING", exceptionType, exceptionMatcher, Catch::ResultDisposition::Normal, expr )
#endif // REQUIRE_THROWS_AS_MATCHING

/* While I despise macro code expansion, this is "better" for us because in the event
of a legit failure, we see the actual line IN-SITU where the test case ACTUALLY IS. */
#define NNG_TESTS_APPEND_STR(m, s) \
    REQUIRE(m != nullptr); \
    REQUIRE(::nng_msg_append(m, s, std::strlen(s)) == 0); \
    REQUIRE(::nng_msg_len(m) == std::strlen(s)); \
    REQUIRE(std::memcmp(::nng_msg_body(m), s, std::strlen(s)) == 0)

#define NNG_TESTS_CHECK_STR(m, s) \
    REQUIRE(m != nullptr); \
    REQUIRE(::nng_msg_len(m) == std::strlen(s)); \
    REQUIRE(std::memcmp(::nng_msg_body(m), s, std::strlen(s)) == 0)

#ifndef SLEEP_FOR
#define SLEEP_FOR(rt) std::this_thread::sleep_for(rt)
#endif // SLEEP_FOR

#endif // CATCH_MACROS_HPP
