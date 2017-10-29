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

#include <nngcpp.h>

#include "../../catch/catch_exception_translations.hpp"
#include "../../catch/catch_nng_exception_matcher.hpp"
#include "../../catch/catch_tags.h"
#include "../../catch/catch_macros.hpp"

#include "../../helpers/basic_fixture.h"
#include "../../helpers/constants.h"

namespace constants {

    const std::string aio_addr = "inproc://aio";

    const std::string hello = "hello";
    const auto hello_buf = to_buffer(hello);
}

TEST_CASE("Asynchronous operations using C++ wrapper", Catch::Tags(
    "async", "operations", "sockets", "cxx").c_str()) {

    using namespace std;
    using namespace nng;
    using namespace nng::protocol;
    using namespace nng::exceptions;
    using namespace constants;
    using namespace Catch::Matchers;
    using O = option_names;

    basic_fixture fixture;

    // TODO: TBD: this is a little bit more than just "given connect pair of sockets..."
    SECTION("Given a connected pair of sockets") {

        unique_ptr<latest_pair_socket> sp1, sp2;

        REQUIRE_NOTHROW(sp1 = make_unique<latest_pair_socket>());
        REQUIRE_NOTHROW(sp2 = make_unique<latest_pair_socket>());

        REQUIRE_NOTHROW(sp1->listen(aio_addr));
        REQUIRE_NOTHROW(sp2->dial(aio_addr));

        SECTION("Asynchronous send and receive works") {

            int tx_count = 0, rx_count = 0;
            unique_ptr<binary_message> bmp;
            unique_ptr<basic_async_service> async_txp, async_rxp;
            const auto timeout = 100ms;

            REQUIRE_NOTHROW(async_txp = make_unique<basic_async_service>([&tx_count]() { tx_count++; }));
            REQUIRE_NOTHROW(async_rxp = make_unique<basic_async_service>([&rx_count]() { rx_count++; }));

            REQUIRE_NOTHROW(async_txp->set(timeout));
            REQUIRE_NOTHROW(async_rxp->set(timeout));

            REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
            REQUIRE_NOTHROW(*bmp << hello);

            REQUIRE(tx_count == 0);
            REQUIRE(rx_count == 0);

            REQUIRE_NOTHROW(sp2->receive_async(async_rxp.get()));

            REQUIRE_NOTHROW(async_txp->retain_message(bmp.get()));
            REQUIRE_NOTHROW(sp1->send_async(async_txp.get()));

            REQUIRE_NOTHROW(async_rxp->wait());

            REQUIRE(async_rxp->success() == true);
            REQUIRE(async_txp->success() == true);

            REQUIRE_NOTHROW(async_rxp->cede_message(bmp.get()));
            REQUIRE_THAT(bmp->GetBody()->Get(), Equals(hello_buf));

            REQUIRE(tx_count == 1);
            REQUIRE(rx_count == 1);
        }
    }

    SECTION("Failure modes work") {

        unique_ptr<latest_pair_socket> sp;
        unique_ptr<basic_async_service> axp;

        int done = 0;
        const auto nominal_timeout = 40ms;

        REQUIRE_NOTHROW(sp = make_unique<latest_pair_socket>());
        REQUIRE_NOTHROW(axp = make_unique<basic_async_service>([&done]() { done++; }));

        const auto& wait_for_done_and_success = [&done, &axp](const error_code_type expected_ec) {
            REQUIRE_NOTHROW(axp->wait());
            REQUIRE(done == 1);
            REQUIRE_THROWS_AS_MATCHING(axp->success(), nng_exception, THROWS_NNG_EXCEPTION(expected_ec));
        };

        REQUIRE(done == 0);

        SECTION("Explicit timeout works") {

            REQUIRE_NOTHROW(axp->set(nominal_timeout));
            REQUIRE_NOTHROW(sp->receive_async(axp.get()));
            wait_for_done_and_success(ec_etimedout);
        }

        SECTION("Default timeout works") {
            REQUIRE_NOTHROW(sp->GetOptions()->SetDuration(O::recv_timeout_duration, nominal_timeout));
            REQUIRE_NOTHROW(sp->receive_async(axp.get()));
            wait_for_done_and_success(ec_etimedout);
        }

        SECTION("Zero timeout works") {
            REQUIRE_NOTHROW(axp->set_timeout(dur_zero));
            REQUIRE_NOTHROW(sp->receive_async(axp.get()));
            wait_for_done_and_success(ec_etimedout);
        }

        SECTION("Cancellation works") {
            REQUIRE_NOTHROW(axp->set_timeout(dur_infinite));
            REQUIRE_NOTHROW(sp->receive_async(axp.get()));
            REQUIRE_NOTHROW(axp->cancel());
            wait_for_done_and_success(ec_ecanceled);
        }
    }
}
