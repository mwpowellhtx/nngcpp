//
// Copyright 2017 Michael W Powell <mwpowellhtx@gmail.com>
// Copyright 2017 Garrett D'Amore <garrett@damore.org>
// Copyright 2017 Capitar IT Group BV <info@capitar.com>
//
// This software is supplied under the terms of the MIT License, a
// copy of which should be located in the distribution where this
// file was obtained (LICENSE.txt).  A copy of the license may also be
// found online at https://opensource.org/licenses/MIT.
//

#include <nngcpp.h>

#include "../catch/catch_nng_exception_matcher.hpp"
#include "../catch/catch_exception_translations.hpp"
#include "../catch/catch_macros.hpp"
#include "../catch/catch_tags.h"

#include "../helpers/constants.h"
#include "../helpers/basic_fixture.h"
#include "../helpers/protocol_boilerplate.hpp"

// Defined for convenience throughout unit testing.
DEFINE_SOCKET_FIXTURE(v0, bus_socket_fixture, bus_socket)

namespace constants {
    const std::string test_addr = "inproc://test";
    const std::string _99bits = "99bits";
    const std::string onthe = "onthe";
    const auto _99bits_buf = to_buffer(_99bits);
    const auto onthe_buf = to_buffer(onthe);
}

TEST_CASE("Verify protocol and peer are correct", Catch::Tags("bus"
    , "v0", "protocol", "peer", "internal", "nng", "cxx").c_str()) {

    using namespace std;
    using namespace nng;
    using namespace nng::protocol::v0;

    protocol_type actual;
    basic_fixture fixture;

    // Yes, we should still verify that Socket construction succeeds.
    unique_ptr<bus_socket_fixture> sp;

    SECTION("Given the created socket") {

        REQUIRE_NOTHROW(sp = make_unique<bus_socket_fixture>());
        REQUIRE(sp.get() != nullptr);

        SECTION("Verify protocol is correct") {
            actual = sp->get_protocol();
            REQUIRE(actual == proto_bus_v0);
            REQUIRE(actual == proto_bus);
        }

        SECTION("Verify peer is correct") {
            actual = sp->get_peer();
            REQUIRE(actual == proto_bus_v0);
            REQUIRE(actual == proto_bus);
        }
    }
}

TEST_CASE("Bus pattern using C++ wrappers", Catch::Tags("bus"
    , "v0", "protocol", "sockets", "pattern", "nng", "cxx").c_str()) {

    using namespace std;
    using namespace std::chrono;
    using namespace nng;
    using namespace nng::protocol;
    using namespace nng::exceptions;
    using namespace constants;
    using namespace Catch::Matchers;
    using O = option_names;

    basic_fixture fixture;

	SECTION("We can create a Bus socket") {

        auto busp = make_unique<latest_bus_socket>();

        REQUIRE(busp != nullptr);

        SECTION("Can close") {
            REQUIRE_NOTHROW(busp.reset());
        }
    }

    SECTION("We can create a linked Bus topology") {

        auto busp1 = make_unique<latest_bus_socket>()
            , busp2 = make_unique<latest_bus_socket>()
            , busp3 = make_unique<latest_bus_socket>()
            ;

        REQUIRE(busp1 != nullptr);
        REQUIRE(busp2 != nullptr);
        REQUIRE(busp3 != nullptr);

        REQUIRE_NOTHROW(busp1->listen(test_addr));
        REQUIRE_NOTHROW(busp2->dial(test_addr));
        REQUIRE_NOTHROW(busp3->dial(test_addr));

        const auto receive_timeout = 50ms;

        REQUIRE_NOTHROW(busp1->GetOptions()->SetDuration(O::recv_timeout_duration, receive_timeout));
        REQUIRE_NOTHROW(busp2->GetOptions()->SetDuration(O::recv_timeout_duration, receive_timeout));
        REQUIRE_NOTHROW(busp3->GetOptions()->SetDuration(O::recv_timeout_duration, receive_timeout));

        SECTION("Messages delivered") {

            auto bmp = make_unique<binary_message>();

            // This is just a poor man's sleep.
            REQUIRE_THROWS_AS_MATCHING(busp1->TryReceive(bmp.get()), nng_exception, THROWS_NNG_EXCEPTION(ec_etimedout));
            REQUIRE_THROWS_AS_MATCHING(busp2->TryReceive(bmp.get()), nng_exception, THROWS_NNG_EXCEPTION(ec_etimedout));
            REQUIRE_THROWS_AS_MATCHING(busp3->TryReceive(bmp.get()), nng_exception, THROWS_NNG_EXCEPTION(ec_etimedout));

            SECTION("Bus2 delivers message to Bus1, Bus3 times out") {

                REQUIRE_NOTHROW(*bmp << _99bits);
                REQUIRE_NOTHROW(busp2->Send(bmp.get()));
                REQUIRE_NOTHROW(busp1->TryReceive(bmp.get()));
                REQUIRE_THAT(bmp->GetBody()->Get(), Equals(_99bits_buf));

                REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
                REQUIRE_THROWS_AS_MATCHING(busp3->TryReceive(bmp.get()), nng_exception, THROWS_NNG_EXCEPTION(ec_etimedout));
            }

            SECTION ("Bus1 delivers message to both Bus2 and Bus3") {

                REQUIRE_NOTHROW(*bmp << onthe);
                REQUIRE_NOTHROW(busp1->Send(bmp.get()));

                REQUIRE_NOTHROW(busp2->TryReceive(bmp.get()));
                // Will compare with the second received message.
                const auto* const msgp1 = bmp->get_message();
                REQUIRE_THAT(bmp->GetBody()->Get(), Equals(onthe_buf));

                REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
                REQUIRE_NOTHROW(busp3->TryReceive(bmp.get()));
                // To demonstrate this is an entirely new message received.
                const auto* const msgp2 = bmp->get_message();
                REQUIRE_THAT(bmp->GetBody()->Get(), Equals(onthe_buf));
                REQUIRE(msgp1 != msgp2);
            }
		}

        SECTION("Can close bus sockets") {

            REQUIRE_NOTHROW(busp1.reset());
            REQUIRE_NOTHROW(busp2.reset());
            REQUIRE_NOTHROW(busp3.reset());

            REQUIRE(busp1 == nullptr);
            REQUIRE(busp2 == nullptr);
            REQUIRE(busp3 == nullptr);
        }
    }
}
