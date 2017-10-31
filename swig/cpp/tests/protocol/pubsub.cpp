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

// Defined for convenience throughout unit testing.
DEFINE_SOCKET_FIXTURE_WITH_RECV_EXPOSURE(v0, pub_socket_fixture, pub_socket)
DEFINE_SOCKET_FIXTURE_WITH_SEND_EXPOSURE(v0, sub_socket_fixture, sub_socket)

namespace constants {

    const std::string test_addr = "inproc://test";

    const std::string abc = "abc";
    const std::string hello = "hello";

    const std::string __empty = "";

    namespace topics {

        const std::string some = "/some/";
        const std::string some_like_it_hot = "/some/like/it/hot";
        const std::string some_day_some_how = "/some/day/some/how";
        const std::string some_do_not_like_it = "some/do/not/like/it";
        const std::string some_like_it_raw = "/some/like/it/raw";

        const auto some_buf = to_buffer(some);
        const auto some_like_it_hot_buf = to_buffer(some_like_it_hot);
        const auto some_day_some_how_buf = to_buffer(some_day_some_how);
        const auto some_do_not_like_it_buf = to_buffer(some_do_not_like_it);
        const auto some_like_it_raw_buf = to_buffer(some_like_it_raw);

        // Yes, this is intentionally different from /some/path/to/topic...
        const std::string somewhere_over_the_rainbow = "/somewhere/over/the/rainbow";
        const auto somewhere_over_the_rainbow_buf = to_buffer(somewhere_over_the_rainbow);
    }
}

TEST_CASE("Verify protocol and peer are correct", Catch::Tags("survey"
    , "v0", "surveyor", "respondent", "protocol", "peer", "internal"
    , "nng", "cxx").c_str()) {

    using namespace std;
    using namespace nng;
    using namespace nng::protocol::v0;

    protocol_type actual;
    basic_fixture fixture;

    SECTION("Verify publisher protocol and peer is correct") {

        unique_ptr<pub_socket_fixture> sp;

        REQUIRE_NOTHROW(sp = make_unique<pub_socket_fixture>());
        REQUIRE(sp.get() == nullptr);

        SECTION("Verify protocol is correct") {
            actual = sp->get_protocol();
            REQUIRE(actual == proto_publisher_v0);
            REQUIRE(actual == proto_publisher);
        }

        SECTION("Verify peer is correct") {
            actual = sp->get_peer();
            REQUIRE(actual == proto_subscriber_v0);
            REQUIRE(actual == proto_subscriber);
        }
    }

    SECTION("Verify subscriber protocol and peer is correct") {

        unique_ptr<sub_socket_fixture> sp;

        REQUIRE_NOTHROW(sp = make_unique<sub_socket_fixture>());
        REQUIRE(sp.get() == nullptr);

        SECTION("Verify protocol is correct") {
            actual = sp->get_protocol();
            REQUIRE(actual == proto_subscriber_v0);
            REQUIRE(actual == proto_subscriber);
        }

        SECTION("Verify peer is correct") {
            actual = sp->get_peer();
            REQUIRE(actual == proto_publisher_v0);
            REQUIRE(actual == proto_publisher);
        }
    }
}

TEST_CASE("Publisher/subscriber pattern using C++ wrapper", Catch::Tags("pub", "sub"
    , "v0", "protocol", "sockets", "patterns", "nng", "cxx").c_str()) {

    using namespace std;
    using namespace constants;
    using namespace nng;
    using namespace nng::protocol;
    using namespace nng::protocol::v0;
    using namespace nng::exceptions;
    using namespace Catch::Matchers;
    using O = option_names;

    basic_fixture fixture;

    // Some of these need to be initialized to avoid garbage results, crash situations, etc.
    size_type sz = 0;

    unique_ptr<binary_message> bmp;
    buffer_vector_type buf;

    unique_ptr<pub_socket_fixture> pubp;
    unique_ptr<sub_socket_fixture> subp;

    SECTION("We can create a publisher socket") {

        REQUIRE_NOTHROW(pubp = make_unique<pub_socket_fixture>());

        SECTION("Receive throws invalid operation exception") {

            REQUIRE_THROWS_AS(pubp->receive(), invalid_operation);
            REQUIRE_THROWS_AS(pubp->receive(sz), invalid_operation);
            REQUIRE_THROWS_AS(pubp->try_receive(bmp.get()), invalid_operation);
            REQUIRE_THROWS_AS(pubp->try_receive(&buf, sz), invalid_operation);
        }

        SECTION("Socket can close") {
            /* This is better than a raw "Close", at least in this scope. Destroying the resource also Closes. */
            REQUIRE_NOTHROW(pubp.reset());
        }
    }

    SECTION("We can create a subscriber socket") {

        REQUIRE_NOTHROW(subp = make_unique<sub_socket_fixture>());

        SECTION("Send throws invalid operation exception") {

            REQUIRE_THROWS_AS(subp->send(bmp.get()), invalid_operation);
            REQUIRE_THROWS_AS(subp->send(&buf), invalid_operation);
            REQUIRE_THROWS_AS(subp->send(&buf, sz), invalid_operation);
        }

        SECTION("Socket can close") {
            // Ditto reset versus raw Close.
            REQUIRE_NOTHROW(subp.reset());
        }
    }

    SECTION("We can create a linked pub/sub pair") {

        REQUIRE_NOTHROW(pubp = make_unique<pub_socket_fixture>());
        REQUIRE_NOTHROW(subp = make_unique<sub_socket_fixture>());

        /* Most applications will usually have the pub listen and sub dial. However, this
        creates a problem for our tests, since we can wind up trying to push data before
        the pipe is fully registered due to the accept running asynchronously. */

        REQUIRE_NOTHROW(subp->listen(test_addr));
        REQUIRE_NOTHROW(pubp->dial(test_addr));
        SLEEP_FOR(20ms); // Time for connecting threads.

        SECTION("Subscriber can subscribe") {

            REQUIRE_NOTHROW(subp->GetOptions()->SetString(O::sub_subscribe, abc));
            REQUIRE_NOTHROW(subp->GetOptions()->SetString(O::sub_subscribe, __empty));

            SECTION("Unsubscribe works") {

                REQUIRE_NOTHROW(subp->GetOptions()->SetString(O::sub_unsubscribe, abc));
                REQUIRE_NOTHROW(subp->GetOptions()->SetString(O::sub_unsubscribe, __empty));

                REQUIRE_THROWS_AS_MATCHING(subp->GetOptions()->SetString(O::sub_unsubscribe, __empty), nng_exception, THROWS_NNG_EXCEPTION(ec_enoent));
                REQUIRE_THROWS_AS_MATCHING(subp->GetOptions()->SetString(O::sub_unsubscribe, hello), nng_exception, THROWS_NNG_EXCEPTION(ec_enoent));
            }
        }

        SECTION("Publisher cannot subscribe") {

            REQUIRE_THROWS_AS_MATCHING(pubp->GetOptions()->SetString(O::sub_subscribe, __empty), nng_exception, THROWS_NNG_EXCEPTION(ec_enotsup));
        }

        SECTION("Subscriber can receive from publisher") {

            REQUIRE_NOTHROW(subp->GetOptions()->SetString(O::sub_subscribe, topics::some));
            REQUIRE_NOTHROW(subp->GetOptions()->SetDuration(O::recv_timeout_duration, 90ms));

            REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
            REQUIRE_NOTHROW(*bmp << topics::some_like_it_hot);
            REQUIRE_NOTHROW(pubp->send(bmp.get()));
            REQUIRE_NOTHROW(subp->try_receive(bmp.get()));
            REQUIRE_THAT(bmp->GetBody()->Get(), Equals(topics::some_like_it_hot_buf));

            REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
            REQUIRE_NOTHROW(*bmp << topics::somewhere_over_the_rainbow);
            REQUIRE_THAT(bmp->GetBody()->Get(), Equals(topics::somewhere_over_the_rainbow_buf));
            REQUIRE_NOTHROW(pubp->send(bmp.get()));
            REQUIRE_THROWS_AS_MATCHING(subp->try_receive(bmp.get()), nng_exception, THROWS_NNG_EXCEPTION(ec_etimedout));

            REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
            REQUIRE_NOTHROW(*bmp << topics::some_day_some_how);
            REQUIRE_NOTHROW(pubp->send(bmp.get()));
            REQUIRE_NOTHROW(subp->try_receive(bmp.get()));
            REQUIRE_THAT(bmp->GetBody()->Get(), Equals(topics::some_day_some_how_buf));
        }

        SECTION("Subscribers without subsciptions do not receive") {

            REQUIRE_NOTHROW(subp->GetOptions()->SetDuration(O::recv_timeout_duration, 90ms));

            REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
            REQUIRE_NOTHROW(*bmp << topics::some_do_not_like_it);
            REQUIRE_NOTHROW(pubp->send(bmp.get()));
            REQUIRE_THROWS_AS_MATCHING(subp->try_receive(bmp.get()), nng_exception, THROWS_NNG_EXCEPTION(ec_etimedout));
        }

        SECTION("Subscribers in raw receive") {

            REQUIRE_NOTHROW(subp->GetOptions()->SetDuration(O::recv_timeout_duration, 90ms));
            REQUIRE_NOTHROW(subp->GetOptions()->SetInt32(O::raw, 1));

            REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
            REQUIRE_NOTHROW(*bmp << topics::some_like_it_raw);
            REQUIRE_NOTHROW(pubp->send(bmp.get()));
            REQUIRE_NOTHROW(subp->try_receive(bmp.get()));
            REQUIRE_THAT(bmp->GetBody()->Get(), Equals(topics::some_like_it_raw_buf));
        }
    }
}
