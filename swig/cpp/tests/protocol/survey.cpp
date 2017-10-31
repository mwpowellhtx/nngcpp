//
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

// Defined for convenience throughout unit testing.
DEFINE_SOCKET_FIXTURE(v0, survey_socket_fixture, survey_socket)
DEFINE_SOCKET_FIXTURE(v0, respond_socket_fixture, respond_socket)

namespace constants {

    const std::string test_addr = "inproc://test";

    const std::string abc = "abc";
    const std::string def = "def";

    const auto abc_buf = to_buffer(abc);
    const auto def_buf = to_buffer(def);
}

TEST_CASE("Verify protocol and peer are correct", Catch::Tags("survey"
    , "v0", "surveyor", "respondent", "protocol", "peer", "internal"
    , "nng", "cxx").c_str()) {

    using namespace std;
    using namespace nng;
    using namespace nng::protocol::v0;

    protocol_type actual;
    basic_fixture fixture;

    SECTION("Verify survey protocol and peer is correct") {

        unique_ptr<survey_socket_fixture> sp;

        REQUIRE_NOTHROW(sp = make_unique<survey_socket_fixture>());
        REQUIRE(sp.get() == nullptr);

        SECTION("Verify protocol is correct") {
            actual = sp->get_protocol();
            REQUIRE(actual == proto_surveyor_v0);
            REQUIRE(actual == proto_surveyor);
        }

        SECTION("Verify peer is correct") {
            actual = sp->get_peer();
            REQUIRE(actual == proto_respondent_v0);
            REQUIRE(actual == proto_respondent);
        }
    }

    SECTION("Verify respondent protocol and peer is correct") {

        unique_ptr<respond_socket_fixture> sp;

        REQUIRE_NOTHROW(sp = make_unique<respond_socket_fixture>());
        REQUIRE(sp.get() == nullptr);

        SECTION("Verify protocol is correct") {
            actual = sp->get_protocol();
            REQUIRE(actual == proto_respondent_v0);
            REQUIRE(actual == proto_respondent);
        }

        SECTION("Verify peer is correct") {
            actual = sp->get_peer();
            REQUIRE(actual == proto_surveyor_v0);
            REQUIRE(actual == proto_surveyor);
        }
    }
}

TEST_CASE("Survey pattern using C++ wrapper", Catch::Tags("surveyor", "respondent"
    , "v0", "protocol", "sockets", "pattern", "nng", "cxx").c_str()) {

    using namespace std;
    using namespace nng;
    using namespace nng::protocol;
    using namespace nng::exceptions;
    using namespace constants;
    using namespace Catch::Matchers;
    using O = option_names;

    basic_fixture fixture;

    unique_ptr<binary_message> bmp;
    unique_ptr<latest_survey_socket> surp;
    unique_ptr<latest_respond_socket> resp;

    SECTION("We can create a Surveyor socket") {

        REQUIRE_NOTHROW(surp = make_unique<latest_survey_socket>());

		SECTION("Receive without survey fails") {

            REQUIRE_NOTHROW(bmp = make_unique<binary_message>(nullptr));
            REQUIRE_THROWS_AS_MATCHING(surp->TryReceive(bmp.get()), nng_exception, THROWS_NNG_EXCEPTION(ec_estate));
		}

		SECTION("Survey without responder times out") {

            REQUIRE_NOTHROW(surp->GetOptions()->SetDuration(O::surveyor_survey_duration, 50ms));

            REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
            REQUIRE_NOTHROW(surp->Send(bmp.get()));
            REQUIRE_THROWS_AS_MATCHING(surp->TryReceive(bmp.get()), nng_exception, THROWS_NNG_EXCEPTION(ec_etimedout));
		}

        SECTION("Socket can close") {
            REQUIRE_NOTHROW(surp.reset());
        }
    }

	SECTION("We can create a Respondent socket") {

        REQUIRE_NOTHROW(resp = make_unique<latest_respond_socket>());

        SECTION("Send fails with no suveyor") {

            REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
            REQUIRE_THROWS_AS_MATCHING(resp->Send(bmp.get()), nng_exception, THROWS_NNG_EXCEPTION(ec_estate));
        }

        SECTION("Socket can close") {
            REQUIRE_NOTHROW(resp.reset());
        }
    }

	SECTION("We can create a linked surveyor/respondent pair") {

        unique_ptr<latest_respond_socket> sockp;

        REQUIRE_NOTHROW(surp = make_unique<latest_survey_socket>());
        REQUIRE_NOTHROW(resp = make_unique<latest_respond_socket>());
        REQUIRE_NOTHROW(sockp = make_unique<latest_respond_socket>());

        //	nng_socket sock;

        REQUIRE_NOTHROW(surp->GetOptions()->SetDuration(O::surveyor_survey_duration, 50ms));

        REQUIRE_NOTHROW(surp->listen(test_addr));
        REQUIRE_NOTHROW(resp->dial(test_addr));

        /* We dial another socket as that will force the earlier dial to have completed FULLY
        This is a hack that only works because our listen logic is single threaded. */

        // TODO: TBD: this is somewhat different from other tests in which there were sleeps. what about waiting for "events"?
        REQUIRE_NOTHROW(sockp->dial(test_addr));
        REQUIRE_NOTHROW(sockp.reset());

        REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
        // ...

        SECTION("Survey works") {

            // ...
            REQUIRE_NOTHROW(*bmp << abc);
            REQUIRE_NOTHROW(surp->Send(bmp.get()));
            REQUIRE_NOTHROW(resp->TryReceive(bmp.get()));
            REQUIRE_THAT(bmp->GetBody()->Get(), Equals(abc_buf));

            REQUIRE_NOTHROW(bmp->GetBody()->TrimLeft(abc.length()));
            REQUIRE_NOTHROW(*bmp << def);
            REQUIRE_NOTHROW(resp->Send(bmp.get()));
            REQUIRE_NOTHROW(surp->TryReceive(bmp.get()));
            REQUIRE_THAT(bmp->GetBody()->Get(), Equals(def_buf));

            REQUIRE_NOTHROW(bmp->retain(nullptr));
            REQUIRE_THROWS_AS_MATCHING(surp->TryReceive(bmp.get()), nng_exception, THROWS_NNG_EXCEPTION(ec_etimedout));

			SECTION("And goes to non-survey state") {

                REQUIRE_NOTHROW(surp->GetOptions()->SetDuration(O::recv_timeout_duration, 200ms));

                // ...
                REQUIRE_THROWS_AS_MATCHING(surp->TryReceive(bmp.get()), nng_exception, THROWS_NNG_EXCEPTION(ec_estate));
			}
		}
	}
}
