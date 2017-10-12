#ifndef CATCH_MACROS_HPP
#define CATCH_MACROS_HPP

#include <catch.hpp>

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

#endif // CATCH_MACROS_HPP
