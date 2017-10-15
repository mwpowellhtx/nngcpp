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

#include "../catch/catch_nng_exception_matcher.hpp"
#include "../catch/catch_exception_translations.hpp"
#include "../catch/catch_macros.hpp"
#include "../helpers/chrono.hpp"

#include <nngcpp.h>

#include <string>

struct bus_fixture {

    virtual ~bus_fixture() {
        ::nng_fini();
    }
};

namespace constants {
    const std::string test_addr = "inproc://test";
    const std::string _99bits = "99bits";
    const std::string onthe = "onthe";
    const nng::messaging::message_base::buffer_vector_type _99bits_buf = { '9','9','b','i','t','s' };
    const nng::messaging::message_base::buffer_vector_type onthe_buf = { 'o','n','t','h','e' };
}

TEST_CASE("Bus pattern tests", "[bus][protocol][sockets][cxx]") {

    using namespace std;
    using namespace std::chrono;
    using namespace nng;
    using namespace nng::protocol;
    using namespace nng::messaging;
    using namespace constants;
    using namespace Catch::Matchers;
    using _opt_ = option_names;

    bus_fixture fixture;


	SECTION("We can create a Bus socket") {

        auto busp = std::make_unique<latest_bus_socket>();

        REQUIRE(busp != nullptr);

        SECTION("Protocols match") {

            REQUIRE(busp->get_protocol() == proto_bus_v0);
            REQUIRE(busp->get_protocol() == proto_bus);

            REQUIRE(busp->get_peer() == proto_bus_v0);
            REQUIRE(busp->get_peer() == proto_bus);
		}

        SECTION("Can close") {
            REQUIRE_NOTHROW(busp.reset());
        }
    }

    SECTION("We can create a linked Bus topology") {

        auto busp1 = std::make_unique<latest_bus_socket>()
            , busp2 = std::make_unique<latest_bus_socket>()
            , busp3 = std::make_unique<latest_bus_socket>()
            ;

        REQUIRE(busp1 != nullptr);
        REQUIRE(busp2 != nullptr);
        REQUIRE(busp3 != nullptr);

        REQUIRE_NOTHROW(busp1->listen(test_addr));
        REQUIRE_NOTHROW(busp2->dial(test_addr));
        REQUIRE_NOTHROW(busp3->dial(test_addr));

        const auto receive_timeout = 50ms;

        REQUIRE_NOTHROW(busp1->set_option_usec(_opt_::receive_timeout_usec, CAST_DURATION_TO_USEC(receive_timeout).count()));
        REQUIRE_NOTHROW(busp2->set_option_usec(_opt_::receive_timeout_usec, CAST_DURATION_TO_USEC(receive_timeout).count()));
        REQUIRE_NOTHROW(busp3->set_option_usec(_opt_::receive_timeout_usec, CAST_DURATION_TO_USEC(receive_timeout).count()));

        SECTION("Messages delivered") {

            auto bmp = std::make_unique<binary_message>();

            // This is just a poor man's sleep.
            REQUIRE_THROWS_AS_MATCHING(busp1->try_receive(bmp.get()), nng_exception, ThrowsNngException(ec_etimedout));
            REQUIRE_THROWS_AS_MATCHING(busp2->try_receive(bmp.get()), nng_exception, ThrowsNngException(ec_etimedout));
            REQUIRE_THROWS_AS_MATCHING(busp3->try_receive(bmp.get()), nng_exception, ThrowsNngException(ec_etimedout));

            SECTION("Bus2 delivers message to Bus1, Bus3 times out") {

                REQUIRE_NOTHROW(*bmp << _99bits);
                REQUIRE_NOTHROW(busp2->send(bmp.get()));
                REQUIRE_NOTHROW(busp1->try_receive(bmp.get()));
                REQUIRE_THAT(bmp->body()->get(), Equals(_99bits_buf));

                REQUIRE_NOTHROW(bmp = std::make_unique<binary_message>());
                REQUIRE_THROWS_AS_MATCHING(busp3->try_receive(bmp.get()), nng_exception, ThrowsNngException(ec_etimedout));
            }

            SECTION ("Bus1 delivers message to both Bus2 and Bus3") {

                REQUIRE_NOTHROW(*bmp << onthe);
                REQUIRE_NOTHROW(busp1->send(bmp.get()));

                REQUIRE_NOTHROW(busp2->try_receive(bmp.get()));
                // Will compare with the second received message.
               const auto* const msgp1 = bmp->get_msgp();
                REQUIRE_THAT(bmp->body()->get(), Equals(onthe_buf));

                REQUIRE_NOTHROW(bmp = std::make_unique<binary_message>());
                REQUIRE_NOTHROW(busp3->try_receive(bmp.get()));
                // To demonstrate this is an entirely new message received.
                const auto* const msgp2 = bmp->get_msgp();
                REQUIRE_THAT(bmp->body()->get(), Equals(onthe_buf));
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
