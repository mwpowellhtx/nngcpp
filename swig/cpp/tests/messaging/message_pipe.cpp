//
// Copyright (c) 2017 Michel W Powell <mwpowellhtx@gmail.com>
//
// This software is supplied under the terms of the MIT License, a
// copy of which should be located in the distribution where this
// file was obtained (LICENSE.txt).  A copy of the license may also be
// found online at https://opensource.org/licenses/MIT.
//

#include <nngcpp.h>

#include "../catch/catch_nng_exception_matcher.hpp"
#include "../catch/catch_exception_translations.hpp"
#include "../catch/catch_tags.h"
#include "../catch/catch_macros.hpp"

#include "../helpers/basic_fixture.h"
#include "../helpers/constants.h"

namespace nng {
    struct message_pipe_fixture : basic_fixture {
    public:

        msg_type* msgp;

        message_pipe_fixture(size_type sz = 0) : basic_fixture(), msgp(nullptr) {
            REQUIRE(::nng_msg_alloc(&msgp, sz) == 0);
            REQUIRE(msgp != nullptr);
        }

        virtual ~message_pipe_fixture() {
            INFO("The fixture is responsible for the 'msgp'");
            REQUIRE(msgp != nullptr);
            ::nng_msg_free(msgp);
        }
    };
}

namespace constants {

    const std::string test_addr = "inproc://test";

    const std::string a_message_was_sent = "a message was sent";
    const auto a_message_was_sent_buf = to_buffer(a_message_was_sent);
}

TEST_CASE("Message pipe subordinates properly", Catch::Tags("message", "pipe"
    , "binary", "subordinate", "messaging", "cxx").c_str()) {

    using namespace std;
    using namespace nng;
    using namespace nng::protocol;
    using namespace nng::exceptions;
    using namespace constants;
    using namespace Catch::Matchers;
    using O = option_names;

    message_pipe_fixture fixture;
    unique_ptr<message_pipe> mpp;

    SECTION("Message pipe constructs properly without associated sockets") {

        unique_ptr<binary_message> bmp;

        REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
        REQUIRE(bmp->has_one() == true);

        SECTION("Message pipe without sockets constructs properly given binary message") {

            REQUIRE_NOTHROW(mpp = make_unique<message_pipe>(bmp.get()));
            REQUIRE(mpp->has_one() == false);

            REQUIRE_NOTHROW(mpp.reset());
        }

        SECTION("Message pipe constructs properly given NNG '::nng_msg' ptr") {

            INFO("Fixture ought to have vetted this, but we will perform the handshake here as well.");
            REQUIRE(fixture.msgp != nullptr);

            REQUIRE_NOTHROW(mpp = make_unique<message_pipe>(fixture.msgp));
            REQUIRE(mpp->has_one() == false);

            REQUIRE_NOTHROW(mpp.reset());
        }
    }

    /* We are not here to test the pair, or the messages being passed, per se.
    However, that is required in order to properly verify message pipes. */

    SECTION("Message pipe constructs properly given associated sockets") {

        unique_ptr<binary_message> sendp, recvp;
        unique_ptr<latest_pair_socket> sp1, sp2;

        REQUIRE_NOTHROW(sendp = make_unique<binary_message>());
        REQUIRE_NOTHROW(recvp = make_unique<binary_message>(nullptr));

        REQUIRE_NOTHROW(sp1 = make_unique<latest_pair_socket>());
        REQUIRE_NOTHROW(sp2 = make_unique<latest_pair_socket>());

        const int expected_recv_buf = 3;
        // Which sets us up to verify some Message Pipe expectations afterwards.
        REQUIRE_NOTHROW(sp2->options()->set_int(O::recv_buf, expected_recv_buf));

        // Using nominal defaults.
        REQUIRE_NOTHROW(sp1->listen(test_addr));
        REQUIRE_NOTHROW(sp2->dial(test_addr));
        // Allow for the listener to catch up.
        SLEEP_FOR(50ms);

        REQUIRE_NOTHROW(*sendp << a_message_was_sent);
        REQUIRE_NOTHROW(sp1->send(sendp.get()));
        REQUIRE(sendp->has_one() == false);
        REQUIRE_NOTHROW(sp2->try_receive(recvp.get()));
        REQUIRE(recvp->has_one() == true);
        REQUIRE_THAT(recvp->body()->get(), Equals(a_message_was_sent_buf));

        // And which here's the rub.
        REQUIRE_NOTHROW(mpp = make_unique<message_pipe>(recvp.get()));
        REQUIRE(mpp->has_one() == true);

        // Perchance to dream, ay, there's the rub! Who doesn't enjoy a little Shakespeare from time to time?
        INFO("When the pointer to the message pipe resets, i.e. is deleted"
            ", the object is destroyed, not to be confused with the pipe itself"
            " resetting , which sets the pipe back on the root message.");

        // We will verify the source Socket's "raw" option via the Message Pipe.
        int actual;

        SECTION("And which Message Pipe Resets properly") {
            // Do not confuse the Message Pipe itself Resetting with the smart pointer.
            REQUIRE_NOTHROW(mpp->reset());
            REQUIRE(mpp->has_one() == false);
        }

        SECTION("And that operating even after originating Message goes away") {

            REQUIRE_NOTHROW(recvp.reset());

            REQUIRE_NOTHROW(mpp->options()->get_int(O::recv_buf, actual));
            REQUIRE(actual == expected_recv_buf);

            REQUIRE_NOTHROW(mpp->close());
        }

        SECTION("And which Throws 'ec_enoent' when Socket channel is destroyed") {

            REQUIRE_NOTHROW(sp2.reset());
            // Operations on the Message Pipe following Channel (Socket) closure are no longer valid.
            REQUIRE_THROWS_AS_MATCHING(mpp->options()->get_int(O::recv_buf, actual), nng_exception, THROWS_NNG_EXCEPTION(ec_enoent));

            SECTION("And which Closing is transparent") {
                /* However, we should be able to "Destroy" the resource, which effectively
                ignores the same condition as being one among several expectations. */
                REQUIRE_NOTHROW(mpp->close());
            }
        }

        SECTION("And which can be associated with another Socket channel") {
            REQUIRE_NOTHROW(sendp = make_unique<binary_message>());
            REQUIRE_NOTHROW(mpp->set(sendp.get()));
            // Remember the Pipe was associated with the Receive Channel, so we expect that Option.
            REQUIRE_NOTHROW(mpp->options()->get_int(O::recv_buf, actual));
            REQUIRE(actual == expected_recv_buf);
        }

        SECTION("And that cleans up properly") {
            // Do not confuse the Smart Pointer resetting with the Message Pipe.
            REQUIRE_NOTHROW(mpp.reset());
        }
    }
}
