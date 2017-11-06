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

        REQUIRE_NOTHROW(sp1->Listen(aio_addr));
        REQUIRE_NOTHROW(sp2->Dial(aio_addr));

        SECTION("Asynchronous send and receive works") {

            int tx_surplus = 0, rx_deficit = 1;
            unique_ptr<binary_message> bmp;
            unique_ptr<basic_async_service> async_txp, async_rxp;
            const auto timeout = 100ms;

            REQUIRE_NOTHROW(async_txp = make_unique<basic_async_service>([&tx_surplus]() { tx_surplus++; }));
            REQUIRE_NOTHROW(async_rxp = make_unique<basic_async_service>([&rx_deficit]() { --rx_deficit; }));

            REQUIRE_NOTHROW(async_txp->GetOptions()->SetTimeoutDuration(timeout));
            REQUIRE_NOTHROW(async_rxp->GetOptions()->SetTimeoutDuration(timeout));

            REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
            REQUIRE_NOTHROW(*bmp << hello);

            REQUIRE(tx_surplus == 0);
            REQUIRE(rx_deficit == 1);

            REQUIRE_NOTHROW(sp2->ReceiveAsync(async_rxp.get()));

            REQUIRE_NOTHROW(*bmp >> *async_txp);
            REQUIRE_NOTHROW(sp1->SendAsync(async_txp.get()));

            REQUIRE_NOTHROW(async_rxp->Wait());

            REQUIRE(async_rxp->Success() == true);
            REQUIRE(async_txp->Success() == true);

            REQUIRE_NOTHROW(*async_rxp >> *bmp);
            REQUIRE_THAT(bmp->GetBody()->Get(), Equals(hello_buf));

            // This is the kind of deficit spending we like.
            REQUIRE(tx_surplus == 1);
            REQUIRE(rx_deficit == 0);
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
            REQUIRE_NOTHROW(axp->Wait());
            REQUIRE(done == 1);
            REQUIRE_THROWS_AS_MATCHING(axp->Success(), nng_exception, THROWS_NNG_EXCEPTION(expected_ec));
        };

        REQUIRE(done == 0);

        SECTION("Explicit timeout works") {

            REQUIRE_NOTHROW(axp->GetOptions()->SetTimeoutDuration(nominal_timeout));
            REQUIRE_NOTHROW(sp->ReceiveAsync(axp.get()));
            wait_for_done_and_success(ec_etimedout);
        }

        SECTION("Default timeout works") {
            REQUIRE_NOTHROW(sp->GetOptions()->SetDuration(O::recv_timeout_duration, nominal_timeout));
            REQUIRE_NOTHROW(sp->ReceiveAsync(axp.get()));
            wait_for_done_and_success(ec_etimedout);
        }

        SECTION("Zero timeout works") {
            REQUIRE_NOTHROW(axp->GetOptions()->SetTimeoutMilliseconds(dur_zero));
            REQUIRE_NOTHROW(sp->ReceiveAsync(axp.get()));
            wait_for_done_and_success(ec_etimedout);
        }

        SECTION("Cancellation works") {
            REQUIRE_NOTHROW(axp->GetOptions()->SetTimeoutMilliseconds(dur_infinite));
            REQUIRE_NOTHROW(sp->ReceiveAsync(axp.get()));
            REQUIRE_NOTHROW(axp->Cancel());
            wait_for_done_and_success(ec_ecanceled);
        }
    }
}
