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

#include "../helpers/basic_fixture.h"
#include "../helpers/constants.h"
#include "../helpers/protocol_boilerplate.hpp"

// Defined for convenience throughout unit testing.
DEFINE_SOCKET_FIXTURE_WITH_RECV_EXPOSURE(v0, push_socket_fixture, push_socket)
DEFINE_SOCKET_FIXTURE_WITH_SEND_EXPOSURE(v0, pull_socket_fixture, pull_socket)

#include <string>

namespace constants {

    const std::string test_addr = "inproc://test";
    const std::string hello = "hello";
    const std::string abc = "abc";
    const std::string def = "def";

    const auto hello_buf = to_buffer(hello);
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

    SECTION("Verify push protocol and peer is correct") {

        unique_ptr<push_socket_fixture> sp;

        REQUIRE_NOTHROW(sp = make_unique<push_socket_fixture>());
        REQUIRE(sp.get() == nullptr);

        SECTION("Verify protocol is correct") {
            actual = sp->get_protocol();
            REQUIRE(actual == proto_pusher_v0);
            REQUIRE(actual == proto_pusher);
        }

        SECTION("Verify peer is correct") {
            actual = sp->get_peer();
            REQUIRE(actual == proto_puller_v0);
            REQUIRE(actual == proto_puller);
        }
    }

    SECTION("Verify pull protocol and peer is correct") {

        unique_ptr<pull_socket_fixture> sp;

        REQUIRE_NOTHROW(sp = make_unique<pull_socket_fixture>());
        REQUIRE(sp.get() == nullptr);

        SECTION("Verify protocol is correct") {
            actual = sp->get_protocol();
            REQUIRE(actual == proto_puller_v0);
            REQUIRE(actual == proto_puller);
        }

        SECTION("Verify peer is correct") {
            actual = sp->get_peer();
            REQUIRE(actual == proto_pusher_v0);
            REQUIRE(actual == proto_pusher);
        }
    }
}

TEST_CASE("Rule out the possibility of invalid operations", Catch::Tags("pipeline"
    , "push", "push", "v0", "invalid", "operations", "send", "receive", "try", "cxx").c_str()) {

    using namespace nng;
    using namespace nng::protocol;
    using namespace nng::protocol::v0;
    using namespace nng::exceptions;

    // Even though we are technically doing nothing with these, we MUST initialize them anyhow.
    size_type sz = 0;
    binary_message* bmp = nullptr;
    buffer_vector_type* bufp = nullptr;

    SECTION("Push sockets cannot receive messages") {

        push_socket_fixture push;

        REQUIRE_THROWS_AS(push.receive(), invalid_operation);
        REQUIRE_THROWS_AS(push.try_receive(bmp), invalid_operation);
        REQUIRE_THROWS_AS(push.receive(sz), invalid_operation);
        REQUIRE_THROWS_AS(push.try_receive(bufp, sz), invalid_operation);
    }

    SECTION("Pull sockets cannot send messages") {

        pull_socket_fixture pull;

        REQUIRE_THROWS_AS(pull.send(bufp), invalid_operation);
        REQUIRE_THROWS_AS(pull.send(bmp), invalid_operation);
        REQUIRE_THROWS_AS(pull.send(bufp, sz), invalid_operation);
    }
}

TEST_CASE("Pipeline (push/pull) pattern using C++ wrapper", Catch::Tags("pipeline"
    , "push", "push", "v0", "protocol", "sockets", "pattern", "nng", "cxx").c_str()) {

    using namespace std;
    using namespace std::chrono;
    using namespace nng;
    using namespace nng::protocol;
    using namespace trx;
    using namespace Catch;
    using namespace Catch::Matchers;
    using namespace constants;
    using O = option_names;

    SECTION("We can create a Push socket") {

        unique_ptr<latest_push_socket> pushsp;

        REQUIRE_NOTHROW(pushsp = make_unique<latest_push_socket>());
        REQUIRE(pushsp != nullptr);

        SECTION("Can close socket") {
            REQUIRE_NOTHROW(pushsp.reset());
            REQUIRE(pushsp == nullptr);
        }
    }

    SECTION("We can create a Pull socket") {

        unique_ptr<latest_pull_socket> pullsp;

        REQUIRE_NOTHROW(pullsp = make_unique<latest_pull_socket>());
        REQUIRE(pullsp != nullptr);

        SECTION("Can close socket") {
            REQUIRE_NOTHROW(pullsp.reset());
            REQUIRE(pullsp == nullptr);
        }
    }
}

TEST_CASE("We can create a linked push/pull pair", Catch::Tags("linked"
    , "push", "push", "v0", "protocol", "sockets", "pattern", "nng", "cxx").c_str()) {

    using namespace std;
    using namespace nng;
    using namespace nng::protocol;
    using namespace constants;
    using namespace Catch::Matchers;

    unique_ptr<latest_push_socket> pushsp;
    unique_ptr<latest_pull_socket> pullsp;
    unique_ptr<latest_push_socket> whatsp;

    REQUIRE_NOTHROW(pushsp = make_unique<latest_push_socket>());
    REQUIRE_NOTHROW(pullsp = make_unique<latest_pull_socket>());
    REQUIRE_NOTHROW(whatsp = make_unique<latest_push_socket>());


    /* Avoid a startup race that the sender be the dialer. Otherwise, you need a
    delay since the server accept is really asynchronous and immediately returns. */

    REQUIRE_NOTHROW(pullsp->listen(test_addr));
    REQUIRE_NOTHROW(pushsp->dial(test_addr));
    REQUIRE_NOTHROW(whatsp->dial(test_addr));

    REQUIRE_NOTHROW(whatsp.reset());

    this_thread::sleep_for(20ms);

    SECTION("Push can send messages, and pull can receive") {

        _Message bm;

        REQUIRE_NOTHROW(bm << hello);
        REQUIRE_NOTHROW(pushsp->send(&bm));
        REQUIRE_NOTHROW(pullsp->try_receive(&bm));
        REQUIRE_THAT(bm.GetBody()->Get(), Equals(hello_buf));
    }

    SECTION("Can close sockets") {

        REQUIRE_NOTHROW(pushsp.reset());
        REQUIRE_NOTHROW(pullsp.reset());
        REQUIRE_NOTHROW(whatsp.reset());

        REQUIRE(pullsp == nullptr);
        REQUIRE(pushsp == nullptr);
        REQUIRE(whatsp == nullptr);
    }
}

TEST_CASE("Load balancing works", Catch::Tags("load", "balancing", "pipeline"
    , "push", "push", "v0", "protocol", "sockets", "pattern", "nng", "cxx").c_str()) {

    using namespace std;
    using namespace std::chrono;
    using namespace nng;
    using namespace nng::protocol;
    using namespace nng::exceptions;
    using namespace constants;
    using namespace Catch::Matchers;
    using O = option_names;

    unique_ptr<latest_push_socket> pushsp;
    unique_ptr<latest_pull_socket> pullsp1, pullsp2, pullsp3;

    REQUIRE_NOTHROW(pushsp = make_unique<latest_push_socket>());
    REQUIRE_NOTHROW(pullsp1 = make_unique<latest_pull_socket>());
    REQUIRE_NOTHROW(pullsp2 = make_unique<latest_pull_socket>());
    REQUIRE_NOTHROW(pullsp3 = make_unique<latest_pull_socket>());

    /* We need to increase the buffer from zero because there is not guarantee that
    the listeners will be present. Which means that they will push back during load
    balancing. Adding a smaller buffer ensures that we can write to each stream even
    when the listeners are not yet running. */

    const int default_buffer_size = 4;

    REQUIRE_NOTHROW(pushsp->GetOptions()->SetInt32(O::recv_buf, default_buffer_size));
    REQUIRE_NOTHROW(pullsp1->GetOptions()->SetInt32(O::recv_buf, default_buffer_size));
    REQUIRE_NOTHROW(pullsp2->GetOptions()->SetInt32(O::recv_buf, default_buffer_size));
    REQUIRE_NOTHROW(pullsp3->GetOptions()->SetInt32(O::recv_buf, default_buffer_size));

    REQUIRE_NOTHROW(pushsp->GetOptions()->SetInt32(O::send_buf, default_buffer_size));
    REQUIRE_NOTHROW(pullsp1->GetOptions()->SetInt32(O::send_buf, default_buffer_size));
    REQUIRE_NOTHROW(pullsp2->GetOptions()->SetInt32(O::send_buf, default_buffer_size));
    REQUIRE_NOTHROW(pullsp3->GetOptions()->SetInt32(O::send_buf, default_buffer_size));

    unique_ptr<binary_message> abcsp, defsp;

    REQUIRE_NOTHROW(abcsp = make_unique<binary_message>());
    REQUIRE_NOTHROW(defsp = make_unique<binary_message>());

    REQUIRE_NOTHROW(*abcsp << abc);
    REQUIRE_NOTHROW(*defsp << def);

    const auto receive_timeout = 100ms;

    REQUIRE_NOTHROW(pullsp1->GetOptions()->SetDuration(O::recv_timeout_duration, receive_timeout));
    REQUIRE_NOTHROW(pullsp2->GetOptions()->SetDuration(O::recv_timeout_duration, receive_timeout));
    REQUIRE_NOTHROW(pullsp3->GetOptions()->SetDuration(O::recv_timeout_duration, receive_timeout));

    REQUIRE_NOTHROW(pushsp->listen(test_addr));
    REQUIRE_NOTHROW(pullsp1->dial(test_addr));
    REQUIRE_NOTHROW(pullsp2->dial(test_addr));
    REQUIRE_NOTHROW(pullsp3->dial(test_addr));
    REQUIRE_NOTHROW(pullsp3->shutdown());

    /* So pullsp3 may not be done accepting yet, but pullsp1 and pullsp2 definitely
    are. Otherwise the server could not have reached accept. The accept logic is
    single threaded, so let's wait that stuff has settled down. */

    this_thread::sleep_for(100ms);

    REQUIRE_NOTHROW(pushsp->send(abcsp.get()));
    REQUIRE_NOTHROW(pushsp->send(defsp.get()));

    REQUIRE_NOTHROW(pullsp1->try_receive(abcsp.get()));
    REQUIRE_THAT(abcsp->GetBody()->Get(), Equals(abc_buf));
    REQUIRE_NOTHROW(pullsp2->try_receive(defsp.get()));
    REQUIRE_THAT(defsp->GetBody()->Get(), Equals(def_buf));

    // Yes, re-make the binary messages.
    REQUIRE_NOTHROW(abcsp = make_unique<binary_message>());
    REQUIRE_NOTHROW(defsp = make_unique<binary_message>());

    REQUIRE_THROWS_AS_MATCHING(pullsp1->try_receive(abcsp.get()), nng_exception, THROWS_NNG_EXCEPTION(ec_etimedout));
    REQUIRE_THROWS_AS_MATCHING(pullsp2->try_receive(defsp.get()), nng_exception, THROWS_NNG_EXCEPTION(ec_etimedout));

    SECTION("Can close sockets") {
        REQUIRE_NOTHROW(pushsp.reset());
        REQUIRE_NOTHROW(pullsp1.reset());
        REQUIRE_NOTHROW(pullsp2.reset());
        REQUIRE_NOTHROW(pullsp3.reset());
    }
}
