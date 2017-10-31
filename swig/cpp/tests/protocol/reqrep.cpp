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
#include "../helpers/protocol_boilerplate.hpp"

DEFINE_SOCKET_FIXTURE(v0, req_socket_fixture, req_socket)
DEFINE_SOCKET_FIXTURE(v0, rep_socket_fixture, rep_socket)

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

TEST_CASE("Verify protocol and peer are correct", Catch::Tags("reqrep"
    , "v0", "req", "requestor", "rep", "replier", "protocol", "peer"
    , "internal", "nng", "cxx").c_str()) {

    using namespace std;
    using namespace nng;
    using namespace nng::protocol::v0;

    protocol_type actual;
    basic_fixture fixture;

    SECTION("Verify requestor protocol and peer is correct") {

        unique_ptr<req_socket_fixture> sp;

        REQUIRE_NOTHROW(sp = make_unique<req_socket_fixture>());
        REQUIRE(sp.get() == nullptr);

        SECTION("Verify protocol is correct") {
            actual = sp->get_protocol();
            REQUIRE(actual == proto_requestor_v0);
            REQUIRE(actual == proto_requestor);
        }

        SECTION("Verify peer is correct") {
            actual = sp->get_peer();
            REQUIRE(actual == proto_replier_v0);
            REQUIRE(actual == proto_replier);
        }
    }

    SECTION("Verify replier protocol and peer is correct") {

        unique_ptr<rep_socket_fixture> sp;

        REQUIRE_NOTHROW(sp = make_unique<rep_socket_fixture>());
        REQUIRE(sp.get() == nullptr);

        SECTION("Verify protocol is correct") {
            actual = sp->get_protocol();
            REQUIRE(actual == proto_replier);
            REQUIRE(actual == proto_replier_v0);
        }

        SECTION("Verify peer is correct") {
            actual = sp->get_peer();
            REQUIRE(actual == proto_requestor_v0);
            REQUIRE(actual == proto_requestor);
        }
    }
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

    unique_ptr<binary_message> bmp;

    unique_ptr<latest_req_socket> reqp;
    unique_ptr<latest_rep_socket> repp;

    SECTION("We can create a request socket") {

        REQUIRE_NOTHROW(reqp = make_unique<latest_req_socket>());

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

            REQUIRE_THROWS_AS_MATCHING(reqp->TryReceive(bmp.get()), nng_exception, THROWS_NNG_EXCEPTION(ec_estate));
		}

        SECTION("Socket can close") {
            // Reset, not Close, as Closing without Reset can cause trouble on the unwind.
            REQUIRE_NOTHROW(reqp.reset());
        }
    }

	SECTION("We can create a reply socket") {

        REQUIRE_NOTHROW(repp = make_unique<latest_rep_socket>());

		SECTION("Send without receive fails") {
            REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
            REQUIRE_THROWS_AS_MATCHING(repp->Send(bmp.get()), nng_exception, THROWS_NNG_EXCEPTION(ec_estate));
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

        REQUIRE_NOTHROW(repp->Listen(test_addr));
        REQUIRE_NOTHROW(reqp->Dial(test_addr));

		SECTION("They can request/reply exchange") {

            REQUIRE_NOTHROW(pingp = make_unique<binary_message>());
            REQUIRE_NOTHROW(pongp = make_unique<binary_message>((::nng_msg*)nullptr));

            REQUIRE_NOTHROW(*pingp << ping);
            REQUIRE_NOTHROW(reqp->Send(pingp.get()));
            // We will use this state to our advantage later on.
            REQUIRE(pingp->HasOne() == false);
            REQUIRE_NOTHROW(repp->TryReceive(pongp.get()));
            REQUIRE(pongp->HasOne() == true);
            REQUIRE_THAT(pongp->GetBody()->Get(), Equals(ping_buf));

            // Interesting, now use the Trim API.
            REQUIRE_NOTHROW(pongp->GetBody()->TrimLeft(ping.length()));
            REQUIRE_NOTHROW(*pongp << pong);
            REQUIRE_NOTHROW(repp->Send(pongp.get()));
            // Here we are using the above assertion to our advantage.
            REQUIRE_NOTHROW(reqp->TryReceive(pingp.get()));
            REQUIRE_THAT(pingp->GetBody()->Get(), Equals(pong_buf));
		}
	}

	SECTION("Request cancellation works") {

        unique_ptr<binary_message> abcp, defp, cmdp;

        REQUIRE_NOTHROW(reqp = make_unique<latest_req_socket>());
        REQUIRE_NOTHROW(repp = make_unique<latest_rep_socket>());

        REQUIRE_NOTHROW(reqp->GetOptions()->SetDuration(O::req_resend_duration, 100ms));
        REQUIRE_NOTHROW(reqp->GetOptions()->SetInt32(O::send_buf, 16));

        REQUIRE_NOTHROW(repp->Listen(test_addr));
        REQUIRE_NOTHROW(reqp->Dial(test_addr));

        REQUIRE_NOTHROW(abcp = make_unique<binary_message>());
        REQUIRE_NOTHROW(*abcp << abc);
        REQUIRE_NOTHROW(defp = make_unique<binary_message>());
        REQUIRE_NOTHROW(*defp << def);
        REQUIRE_NOTHROW(cmdp = make_unique<binary_message>(nullptr));

        REQUIRE_NOTHROW(reqp->Send(abcp.get()));
        REQUIRE_NOTHROW(reqp->Send(defp.get()));
        REQUIRE_NOTHROW(repp->TryReceive(cmdp.get()));
        REQUIRE_NOTHROW(cmdp->HasOne() == true);

        REQUIRE_NOTHROW(repp->Send(cmdp.get()));
        REQUIRE_NOTHROW(repp->TryReceive(cmdp.get()));
        REQUIRE_NOTHROW(repp->Send(cmdp.get()));
        REQUIRE_NOTHROW(reqp->TryReceive(cmdp.get()));

        REQUIRE_THAT(cmdp->GetBody()->Get(), Equals(def_buf));
	}
}
