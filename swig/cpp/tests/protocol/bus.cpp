//
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
#include <chrono>

//#define APPENDSTR(m, s) nng_msg_append(m, s, strlen(s))
//#define CHECKSTR(m, s)                   \
//	So(nng_msg_len(m) == strlen(s)); \
//	So(memcmp(nng_msg_body(m), s, strlen(s)) == 0)

namespace constants {
    const std::string test_addr = "inproc://test";
    const std::string _99bits = "99bits";
    const std::string onthe = "onthe";
}

TEST_CASE("Bus pattern tests", "[protocol][bus]") {

    using namespace std;
    using namespace std::chrono;
    using namespace nng;
    using namespace nng::protocol;
    using namespace constants;
    using namespace Catch::Matchers;
    using _opt_ = option_names;

    session _session_;


	SECTION("We can create a Bus socket") {

        shared_ptr<latest_bus_socket> sp;

        REQUIRE_NOTHROW(sp = _session_.create_bus_socket());

		SECTION("Protocols match") {

            REQUIRE(sp->get_protocol() == proto_bus_v0);
            REQUIRE(sp->get_protocol() == proto_bus);

            REQUIRE(sp->get_peer() == proto_bus_v0);
            REQUIRE(sp->get_peer() == proto_bus);
		}

        REQUIRE_NOTHROW(_session_.remove_bus_socket(sp.get()));

        REQUIRE_NOTHROW(sp.reset());
    }

	SECTION("We can create a linked Bus topology") {

        shared_ptr<latest_bus_socket> sp1, sp2, sp3;

        REQUIRE_NOTHROW(sp1 = _session_.create_bus_socket());
        REQUIRE_NOTHROW(sp2 = _session_.create_bus_socket());
        REQUIRE_NOTHROW(sp3 = _session_.create_bus_socket());

        REQUIRE(sp1 != nullptr);
        REQUIRE(sp2 != nullptr);
        REQUIRE(sp3 != nullptr);

        SECTION("First Bus Listens, second and third Buses Dial") {
            REQUIRE_NOTHROW(sp1->listen(test_addr));
            REQUIRE_NOTHROW(sp2->dial(test_addr));
            REQUIRE_NOTHROW(sp3->dial(test_addr));
        }

        SECTION("Configure Bus receive timeouts to 50 milliseconds") {
            INFO("TODO: TBD: currently failing to Listen: crashes with segmentation fault.");
            const auto receive_timeout = 50ms;
            REQUIRE_NOTHROW(sp1->set_option_usec(_opt_::receive_timeout_usec, CAST_DURATION_TO_USEC(receive_timeout).count()));
            REQUIRE_NOTHROW(sp2->set_option_usec(_opt_::receive_timeout_usec, CAST_DURATION_TO_USEC(receive_timeout).count()));
            REQUIRE_NOTHROW(sp3->set_option_usec(_opt_::receive_timeout_usec, CAST_DURATION_TO_USEC(receive_timeout).count()));
        }

        SECTION("Messages delivered") {

            string r1, r2, r3;
            socket::receive_size_type sz1, sz2, sz3;

            SECTION("Conduct a poor man's timeout") {

                // TODO: TBD: these calls are blocking. But we are also trying to receive a "string", not using the send/recv-msg API...
                sz1 = sz2 = sz3 = 1;

                // This is just a poor man's sleep.
                REQUIRE_THROWS_AS_MATCHING(sp1->try_receive(r1, sz1), nng_exception, ThrowsNngException(ec_etimedout));
                REQUIRE_THROWS_AS_MATCHING(sp2->try_receive(r2, sz2), nng_exception, ThrowsNngException(ec_etimedout));
                REQUIRE_THROWS_AS_MATCHING(sp3->try_receive(r3, sz3), nng_exception, ThrowsNngException(ec_etimedout));
            }

            SECTION("First Client Sends to Server, second Client does not Receive") {

                FAIL();

                REQUIRE_NOTHROW(sp2->send(_99bits));

                r1.resize(sz1 = _99bits.length());

                REQUIRE_NOTHROW(sp1->try_receive(r1, sz1));
                REQUIRE_THROWS_AS_MATCHING(sp3->try_receive(r2, sz2), nng_exception, ThrowsNngException(ec_etimedout));
                REQUIRE_THAT(r1, Equals(_99bits));
            }

            SECTION ("Server Sends to the Clients, both Clients receive") {

                FAIL();

                REQUIRE_NOTHROW(sp1->send(onthe));

                r2.resize(sz2 = onthe.length());

                REQUIRE_NOTHROW(sp2->try_receive(r2, sz2));
                REQUIRE_THAT(r2, Equals(onthe));

                r3.resize(sz3 = onthe.length());

                REQUIRE_NOTHROW(sp3->try_receive(r3, sz3));
                REQUIRE_THAT(r3, Equals(onthe));
            }
		}

        REQUIRE_NOTHROW(_session_.remove_bus_socket(sp1.get()));
        REQUIRE_NOTHROW(_session_.remove_bus_socket(sp2.get()));
        REQUIRE_NOTHROW(_session_.remove_bus_socket(sp3.get()));

        REQUIRE_NOTHROW(sp1.reset());
        REQUIRE_NOTHROW(sp2.reset());
        REQUIRE_NOTHROW(sp3.reset());
    }
}
