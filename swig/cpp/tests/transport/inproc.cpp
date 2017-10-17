//
// Copyright (c) 2017 Michael W Powell <mwpowellhtx@gmail.com>
// Copyright 2017 Garrett D'Amore <garrett@damore.org>
// Copyright 2017 Capitar IT Group BV <info@capitar.com>
//
// This software is supplied under the terms of the MIT License, a
// copy of which should be located in the distribution where this
// file was obtained (LICENSE.txt).  A copy of the license may also be
// found online at https://opensource.org/licenses/MIT.
//

#include <catch.hpp>

#include "transport.h"

namespace constants {
    const std::string test_addr_base = "inproc://test_";
}

TEST_CASE("Inproc transport tests", "[inproc][transport][nng][cxx]") {

    using namespace nng;
    using namespace constants;

    // Everything is tucked away in the "fixture" in this instance.
    transport_fixture fixture(test_addr_base);

    fixture.run_all();
}
