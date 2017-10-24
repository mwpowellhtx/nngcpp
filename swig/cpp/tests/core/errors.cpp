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

// Loosely based on the underlying NNG unit tests. We ought to be able to confirm along the same lines.
TEST_CASE("Error messages work", "[errors]") {

    // TODO: TBD: pretty close to an end to end build including testing: now need to link in library(ies) supporting C++
    //Severity	Code	Description	Project	File	Line	Suppression State
    //    Error	LNK2019	unresolved external symbol "public: static class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > __cdecl nng::nng_exception::strerror(int)" (? strerror@nng_exception@nng@@SA?AV ? $basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@H@Z) referenced in function "void __cdecl ____C_A_T_C_H____T_E_S_T____0(void)" (? ____C_A_T_C_H____T_E_S_T____0@@YAXXZ)	errors	G : \Source\Spikes\nanomsg\cppnngswig - working\build\swig\cpp\tests\errors.obj	1

    using namespace std;
    using namespace nng;
    using namespace trx::exceptions;

    SECTION("Known errors work") {
        REQUIRE(true);
        REQUIRE(exception_utils::strerror(ec_eclosed) == "Object closed");
        REQUIRE(exception_utils::strerror(ec_etimedout) == "Timed out");
    }

    SECTION("We always get a valid error") {
        for (int errnum = 1; errnum < 0x1000000; errnum = errnum * 2 + 100) {
            REQUIRE(exception_utils::strerror(errnum) != "");
        }
    }

    SECTION("System errors work") {
        REQUIRE(exception_utils::strerror(static_cast<int>(ec_esyserr) + ENOENT) == ::strerror(ENOENT));
        REQUIRE(exception_utils::strerror(static_cast<int>(ec_esyserr) + EINVAL) == ::strerror(EINVAL));
    }
}
