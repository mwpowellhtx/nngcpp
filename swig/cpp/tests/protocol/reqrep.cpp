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

#include "../catch/catch_nng_exception_matcher.hpp"
#include "../catch/catch_exception_translations.hpp"
#include "../catch/catch_macros.hpp"
#include "../catch/catch_tags.h"

#include "../helpers/basic_fixture.h"
#include "../helpers/constants.h"

namespace constants {

    const std::string test_addr = "inproc://test";

    const std::string ping = "ping";
    const std::string pong = "pong";

    const std::string abc = "abc";
    const std::string def = "def";

    const std::string __empty = "";

    const auto ping_buf = to_buffer(ping);
    const auto pong_buf = to_buffer(pong);

    const auto abc_buf = to_buffer(abc);
    const auto def_buf = to_buffer(def);
}

TEST_CASE("Request/reply pattern using C++ wrapper", Catch::Tags("req", "rep"
    , "v0", "protocol", "sockets", "patterns", "nng", "cxx").c_str()) {

    using namespace std;
    using namespace nng;
    using namespace nng::protocol;
    using namespace nng::exceptions;
    using namespace constants;
    using namespace Catch::Matchers;
    using O = option_names;

    protocol_type actual_proto, actual_peer;

    unique_ptr<binary_message> bmp;

    unique_ptr<latest_req_socket> reqp;
    unique_ptr<latest_rep_socket> repp;

    SECTION("We can create a request socket") {

        REQUIRE_NOTHROW(reqp = make_unique<latest_req_socket>());

		SECTION("Protocols match") {

            REQUIRE_NOTHROW(actual_proto = reqp->get_protocol());

            REQUIRE(actual_proto == proto_requestor);
            REQUIRE(actual_proto == proto_requestor_v0);

            REQUIRE_NOTHROW(actual_peer = reqp->get_peer());

            REQUIRE(actual_peer == proto_replier);
            REQUIRE(actual_peer == proto_replier_v0);
		}

		SECTION("Request resend time option works") {

            // Set the timeout.
            REQUIRE_NOTHROW(reqp->GetOptions()->SetDuration(O::req_resend_duration, 10ms));
            // TODO: TBD: it is unit tests like this that really deserve a more focused unit test on just options alone...
            // Check invalid size.
            REQUIRE_THROWS_AS_MATCHING(reqp->GetOptions()->SetString(O::req_resend_duration, __empty), nng_exception, THROWS_NNG_EXCEPTION(ec_einval));
		}

		SECTION("Receive socket without send socket fails") {

            // TODO: TBD: really needs its own unit tests for the operation alone: verify in the binary message tests...
            REQUIRE_NOTHROW(bmp = make_unique<binary_message>(static_cast<::nng_msg*>(nullptr)));

            REQUIRE_THROWS_AS_MATCHING(reqp->try_receive(bmp.get()), nng_exception, THROWS_NNG_EXCEPTION(ec_estate));
		}

        SECTION("Socket can close") {
            // Reset, not Close, as Closing without Reset can cause trouble on the unwind.
            REQUIRE_NOTHROW(reqp.reset());
        }
    }

	SECTION("We can create a reply socket") {

        REQUIRE_NOTHROW(repp = make_unique<latest_rep_socket>());

		SECTION("Protocols match") {

            REQUIRE_NOTHROW(actual_proto = repp->get_protocol());

            REQUIRE(actual_proto == proto_replier);
            REQUIRE(actual_proto == proto_replier_v0);

            REQUIRE_NOTHROW(actual_peer = repp->get_peer());

            REQUIRE(actual_peer == proto_requestor);
            REQUIRE(actual_peer == proto_requestor_v0);
        }

		SECTION("Send without receive fails") {
            REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
            REQUIRE_THROWS_AS_MATCHING(repp->send(bmp.get()), nng_exception, THROWS_NNG_EXCEPTION(ec_estate));
		}

		SECTION("Cannot set resend time option") {
            REQUIRE_THROWS_AS_MATCHING(repp->GetOptions()->SetDuration(O::req_resend_duration, 1ms), nng_exception, THROWS_NNG_EXCEPTION(ec_enotsup));
		}

        SECTION("Socket can close") {
            // Reset, not Close, as Closing without Reset can cause trouble on the unwind.
            REQUIRE_NOTHROW(repp.reset());
        }
    }

	SECTION("We can create a linked request/reply pair") {

        unique_ptr<binary_message> pingp, pongp;

        REQUIRE_NOTHROW(reqp = make_unique<latest_req_socket>());
        REQUIRE_NOTHROW(repp = make_unique<latest_rep_socket>());

        REQUIRE_NOTHROW(repp->listen(test_addr));
        REQUIRE_NOTHROW(reqp->dial(test_addr));

		SECTION("They can request/reply exchange") {

            REQUIRE_NOTHROW(pingp = make_unique<binary_message>());
            REQUIRE_NOTHROW(pongp = make_unique<binary_message>((::nng_msg*)nullptr));

            REQUIRE_NOTHROW(*pingp << ping);
            REQUIRE_NOTHROW(reqp->send(pingp.get()));
            // We will use this state to our advantage later on.
            REQUIRE(pingp->HasOne() == false);
            REQUIRE_NOTHROW(repp->try_receive(pongp.get()));
            REQUIRE(pongp->HasOne() == true);
            REQUIRE_THAT(pongp->GetBody()->Get(), Equals(ping_buf));

            // Interesting, now use the Trim API.
            REQUIRE_NOTHROW(pongp->GetBody()->TrimLeft(ping.length()));
            REQUIRE_NOTHROW(*pongp << pong);
            REQUIRE_NOTHROW(repp->send(pongp.get()));
            // Here we are using the above assertion to our advantage.
            REQUIRE_NOTHROW(reqp->try_receive(pingp.get()));
            REQUIRE_THAT(pingp->GetBody()->Get(), Equals(pong_buf));
		}
	}

	SECTION("Request cancellation works") {

        unique_ptr<binary_message> abcp, defp, cmdp;

        REQUIRE_NOTHROW(reqp = make_unique<latest_req_socket>());
        REQUIRE_NOTHROW(repp = make_unique<latest_rep_socket>());

        REQUIRE_NOTHROW(reqp->GetOptions()->SetDuration(O::req_resend_duration, 100ms));
        REQUIRE_NOTHROW(reqp->GetOptions()->SetInt32(O::send_buf, 16));

        REQUIRE_NOTHROW(repp->listen(test_addr));
        REQUIRE_NOTHROW(reqp->dial(test_addr));

        REQUIRE_NOTHROW(abcp = make_unique<binary_message>());
        REQUIRE_NOTHROW(*abcp << abc);
        REQUIRE_NOTHROW(defp = make_unique<binary_message>());
        REQUIRE_NOTHROW(*defp << def);
        REQUIRE_NOTHROW(cmdp = make_unique<binary_message>(nullptr));

        REQUIRE_NOTHROW(reqp->send(abcp.get()));
        REQUIRE_NOTHROW(reqp->send(defp.get()));
        REQUIRE_NOTHROW(repp->try_receive(cmdp.get()));
        REQUIRE_NOTHROW(cmdp->HasOne() == true);

        REQUIRE_NOTHROW(repp->send(cmdp.get()));
        REQUIRE_NOTHROW(repp->try_receive(cmdp.get()));
        REQUIRE_NOTHROW(repp->send(cmdp.get()));
        REQUIRE_NOTHROW(reqp->try_receive(cmdp.get()));

        REQUIRE_THAT(cmdp->GetBody()->Get(), Equals(def_buf));
	}
}
