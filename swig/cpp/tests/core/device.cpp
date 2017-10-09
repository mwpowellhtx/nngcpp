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

#include <nngcpp.h>

#include <string>
#include <chrono>
#include <ostream>

#include "../catch/catch_exception_translations.hpp"

namespace constants {
    const std::string dev1_addr = "inproc://dev1";
    const std::string dev2_addr = "inproc://dev2";
}

#define CAST_DURATION_TO_USEC std::chrono::duration_cast<microseconds>

TEST_CASE("Test that device functions properly", "[device]") {

    using namespace Catch;
    using namespace Catch::Matchers;
    using namespace std;
    using namespace std::chrono;
    using namespace nng;
    using namespace nng::protocol;
    using namespace constants;
    using _opt_ = option_names;

    session _session_;

    SECTION("nng::protocol::v1::pair_socket based device functions properly") {

		SECTION("We can create a nng::protocol::v1::pair_socket device") {

            shared_ptr<latest_pair_socket> sp1, sp2;

            REQUIRE_NOTHROW(sp1 = _session_.create_pair_socket());
            REQUIRE_NOTHROW(sp2 = _session_.create_pair_socket());

            // TODO: TBD: in the grand scheme of things, I'm not really sure what purpose these two assertions are serving...
            REQUIRE_NOTHROW(sp1->set_option_int(_opt_::raw, 1));
            REQUIRE_NOTHROW(sp2->set_option_int(_opt_::raw, 1));

            shared_ptr<device> devp;

            REQUIRE_NOTHROW(devp = _session_.create_device(sp1.get(), sp2.get(), true));

            REQUIRE_NOTHROW(sp1->listen(dev1_addr));
            REQUIRE_NOTHROW(sp2->listen(dev2_addr));

            shared_ptr<latest_pair_socket> es1, es2;

            REQUIRE_NOTHROW(es1 = _session_.create_pair_socket());
            REQUIRE_NOTHROW(es2 = _session_.create_pair_socket());

            REQUIRE_NOTHROW(es1->dial(dev1_addr));
            REQUIRE_NOTHROW(es2->dial(dev1_addr));

            const auto timeout = 1000ms;

            REQUIRE_NOTHROW(es1->set_option_usec(_opt_::receive_timeout_usec, CAST_DURATION_TO_USEC(timeout).count()));
            REQUIRE_NOTHROW(es2->set_option_usec(_opt_::receive_timeout_usec, CAST_DURATION_TO_USEC(timeout).count()));

            this_thread::sleep_for(100ms);

            string actual;

            SECTION("Device can send and receive") {

                SECTION("Device can send") {

                    const string value = "ALPHA", expected = value;

                    (actual = "").resize(expected.length() + 1);

                    // Send from 1->2 via the listening Device sockets.
                    REQUIRE_NOTHROW(es1->send(value));
                    REQUIRE_NOTHROW(es2->try_receive(actual, actual.size()));
                    REQUIRE(actual == expected);
                }

                SECTION("Device can receive") {

                    const string value = "OMEGA", expected = value;

                    (actual = "").resize(expected.length() + 1);

                    // Now respond from 2->1 via the listening Device sockets.
                    REQUIRE_NOTHROW(es2->send(value));
                    REQUIRE_NOTHROW(es1->try_receive(actual, actual.size()));
                    REQUIRE(actual == expected);
                }
			}
		}
	}
}
