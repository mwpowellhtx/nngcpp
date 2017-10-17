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

#include "transport.h"

namespace constants {
    // TODO: TBD: I wonder if this ultimately refers to actually /tmp/nng_ipc_test_ in the file path?
    // TODO: TBD: that would be on Linux, I'm assuming; what about on Windows? is the implementation "there" yet?
    //const std::string test_addr_base = "ipc:///tmp/nng_ipc_test_";
    const std::string test_addr_base = "ipc:///tmp/nng_ipc_test_";
}

TEST_CASE("IPC transport tests in C", "[ipc][transport][nng][c]") {

    using namespace nng;
    using namespace constants;

    // Everything is tucked away in the "fixture" in this instance.
    c_style_transport_fixture fixture(test_addr_base);

    fixture.run_all();
}

TEST_CASE("IPC transport tests using C++ wrapper", "[ipc][transport][nng][cxx]") {

    using namespace nng;
    using namespace constants;

    // Everything is tucked away in the "fixture" in this instance.
    transport_fixture fixture(test_addr_base);

    fixture.run_all();
}
