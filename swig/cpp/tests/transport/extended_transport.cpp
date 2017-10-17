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

#include "../catch/catch_nng_exception_matcher.hpp"
#include "../catch/catch_exception_translations.hpp"
#include "../catch/catch_macros.hpp"
#include "../helpers/constants.h"

#include "extended_transport.h"

#include <cstdlib>
#include <sstream>
#include <algorithm>

namespace constants {

    const std::string props = "props";

    const auto props_buf = to_buffer(props);
}

namespace nng {

    // TODO: TBD: we could potentially accept command line arguments for the PORT, but I really fail to see the need
    extended_transport_fixture::extended_transport_fixture(const std::string& base_addr
        , const run_property_tests_func& property_tests)
        : transport_fixture(base_addr)
        , _test_properties(property_tests) {
    }

    extended_transport_fixture::~extended_transport_fixture() {
    }

    void extended_transport_fixture::run_all() {
        transport_fixture::run_all();
    }

    void extended_transport_fixture::run_all(const std::string& addr) {

        transport_fixture::run_all(addr);

        run_property_tests(addr);
    }

    void extended_transport_fixture::run_property_tests(const std::string& addr) {

        using namespace std;
        using namespace messaging;
        using namespace protocol;
        using namespace constants;
        using namespace Catch::Matchers;

        SECTION("Run property tests") {

            unique_ptr<listener> lp;
            unique_ptr<dialer> dp;

            unique_ptr<binary_message> sendp, recvp;

            REQUIRE_NOTHROW(lp = make_unique<listener>());
            REQUIRE_NOTHROW(dp = make_unique<dialer>());

            REQUIRE_NOTHROW(_repp->listen(addr, lp.get()));
            REQUIRE(lp->has_one() == true);
            REQUIRE_NOTHROW(_reqp->dial(addr, dp.get()));
            REQUIRE(dp->has_one() == true);

            // Wait for listener to catch up since it may be slightly behind.
            SLEEP_FOR(20ms);

            REQUIRE_NOTHROW(sendp = make_unique<binary_message>());
            REQUIRE_NOTHROW(recvp = make_unique<binary_message>(nullptr));
            REQUIRE(recvp->has_one() == false);
            REQUIRE_NOTHROW(*sendp << props);

            REQUIRE_NOTHROW(_reqp->send(sendp.get()));
            REQUIRE_NOTHROW(_repp->try_receive(recvp.get()));
            REQUIRE(recvp->has_one() == true);
            REQUIRE_THAT(recvp->body()->get(), Equals(props_buf));

            _test_properties(recvp.get(), lp.get(), dp.get());
        }
    }
}
