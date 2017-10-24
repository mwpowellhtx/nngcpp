//
// Copyright (c) 2017 Michael W Powell <mwpowellhtx@gmail.com>
//
// This software is supplied under the terms of the MIT License, a
// copy of which should be located in the distribution where this
// file was obtained (LICENSE.txt).  A copy of the license may also be
// found online at https://opensource.org/licenses/MIT.
//

#include <catch.hpp>
#include <nngcpp.h>

#include <string>
#include <cstring>

namespace constants {
    const std::string obj_closed = "Object closed";
    const std::string timed_out = "Timed out";
    const std::string __empty = "";
}

// Loosely based on the underlying NNG unit tests. We ought to be able to confirm along the same lines.
TEST_CASE("Error messages work", "[errors]") {

    // TODO: TBD: pretty close to an end to end build including testing: now need to link in library(ies) supporting C++
    //Severity	Code	Description	Project	File	Line	Suppression State
    //    Error	LNK2019	unresolved external symbol "public: static class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > __cdecl nng::nng_exception::strerror(int)" (? strerror@nng_exception@nng@@SA?AV ? $basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@H@Z) referenced in function "void __cdecl ____C_A_T_C_H____T_E_S_T____0(void)" (? ____C_A_T_C_H____T_E_S_T____0@@YAXXZ)	errors	G : \Source\Spikes\nanomsg\cppnngswig - working\build\swig\cpp\tests\errors.obj	1

    using namespace std;
    using namespace nng;
    using namespace constants;
    using namespace Catch::Matchers;

    SECTION("Known errors work") {
        REQUIRE(true);
        REQUIRE_THAT(STRERROR(ec_eclosed), Equals(obj_closed));
        REQUIRE_THAT(STRERROR(ec_etimedout), Equals(timed_out));
    }

    SECTION("We always get a valid error") {
        for (int errnum = 1; errnum < 0x1000000; errnum = errnum * 2 + 100) {
            REQUIRE_THAT(STRERROR(errnum), Not(Equals(__empty)));
        }
    }

    // TODO: TBD: all roads to not lead through Rome; in this case they lead through ::strerror, that to say I'm not sure how much of a test this really is any more...
    SECTION("System errors work") {
        const auto syserr_base = static_cast<int>(ec_esyserr);
        REQUIRE_THAT(STRERROR(syserr_base + ENOENT), Not(Equals(__empty)));
        REQUIRE_THAT(STRERROR(syserr_base + EINVAL), Not(Equals(__empty)));
    }
}
