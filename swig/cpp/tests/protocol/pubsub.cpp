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

#include "../catch/catch_nng_exception_matcher.hpp"
#include "../catch/catch_exception_translations.hpp"
#include "../catch/catch_macros.hpp"
#include "../helpers/basic_fixture.h"
#include "../helpers/constants.h"
#include "../helpers/chrono.hpp"

#include <nngcpp.h>

namespace nng {
    namespace protocol {
        namespace v0 {

            class pub_socket_fixture : public pub_socket {
            public:

                pub_socket_fixture() : pub_socket() {}

                virtual ~pub_socket_fixture() {}

                std::unique_ptr<binary_message_type> receive(flag_type flags = flag_none) {
                    return pub_socket::receive(flags);
                }

                int try_receive(binary_message_type* const bmp, flag_type flags = flag_none) {
                    return pub_socket::try_receive(bmp, flags);
                }

                buffer_vector_type receive(size_type& sz, flag_type flags = flag_none) {
                    return pub_socket::receive(sz, flags);
                }

                int try_receive(buffer_vector_type* const bufp, size_type& sz, flag_type flags = flag_none) {
                    return pub_socket::try_receive(bufp, sz, flags);
                }
            };

            class sub_socket_fixture : public sub_socket {
            public:

                sub_socket_fixture() : sub_socket() {}

                virtual ~sub_socket_fixture() {}

                void send(binary_message_type* const bmp, flag_type flags = flag_none) {
                    sub_socket::send(bmp, flags);
                }

                int send(const buffer_vector_type* const bufp, flag_type flags = flag_none) {
                    return sub_socket::send(bufp, flags);
                }

                int send(const buffer_vector_type* const bufp, size_type sz, flag_type flags = flag_none) {
                    return sub_socket::send(bufp, sz, flags);
                }
            };
        }

        typedef v0::pub_socket_fixture latest_pub_socket_fixture;
        typedef v0::sub_socket_fixture latest_sub_socket_fixture;
    }
}

namespace constants {

    const std::string test_addr = "inproc://test";

    const std::string __empty = "";
    const std::string abc = "abc";
    const std::string hello = "hello";

    namespace topics {

        const std::string some = "/some/";
        const std::string some_like_it_hot = "/some/like/it/hot";
        const std::string some_day_some_how = "/some/day/some/how";
        const std::string some_do_not_like_it = "some/do/not/like/it";
        const std::string some_like_it_raw = "/some/like/it/raw";

        const TO_BUFFER_RETVAL some_buf = to_buffer(some);
        const TO_BUFFER_RETVAL some_like_it_hot_buf = to_buffer(some_like_it_hot);
        const TO_BUFFER_RETVAL some_day_some_how_buf = to_buffer(some_day_some_how);
        const TO_BUFFER_RETVAL some_do_not_like_it_buf = to_buffer(some_do_not_like_it);
        const TO_BUFFER_RETVAL some_like_it_raw_buf = to_buffer(some_like_it_raw);

        // Yes, this is intentionally different from /some/path/to/topic...
        const std::string somewhere_over_the_rainbow = "/somewhere/over/the/rainbow";
        const TO_BUFFER_RETVAL somewhere_over_the_rainbow_buf = to_buffer(somewhere_over_the_rainbow);
    }
}

TEST_CASE("Publisher/subscriber pattern using C++ wrapper", "[pubsub][v0][protocol][sockets][cxx]") {

    using namespace std;
    using namespace constants;
    using namespace nng;
    using namespace nng::protocol;
    using namespace nng::messaging;
    using namespace trx;
    using namespace Catch::Matchers;
    using O = option_names;

    basic_fixture fixture;

    // Some of these need to be initialized to avoid garbage results, crash situations, etc.
    socket::size_type sz = 0;
    protocol_type actual_proto, actual_peer;

    unique_ptr<binary_message> bmp;
    binary_message::buffer_vector_type buf;

    unique_ptr<latest_pub_socket_fixture> pubp;
    unique_ptr<latest_sub_socket_fixture> subp;

    SECTION("We can create a publisher socket") {

        REQUIRE_NOTHROW(pubp = make_unique<latest_pub_socket_fixture>());

        SECTION("Protocols match") {

            REQUIRE_NOTHROW(actual_proto = pubp->get_protocol());

            REQUIRE(actual_proto == proto_publisher);
            REQUIRE(actual_proto == proto_publisher_v0);

            REQUIRE_NOTHROW(actual_peer = pubp->get_peer());

            REQUIRE(actual_peer == proto_subscriber);
            REQUIRE(actual_peer == proto_subscriber_v0);
        }

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

        REQUIRE_NOTHROW(subp = make_unique<latest_sub_socket_fixture>());

        SECTION("Protocols match") {

            REQUIRE_NOTHROW(actual_proto = subp->get_protocol());

            REQUIRE(actual_proto == proto_subscriber);
            REQUIRE(actual_proto == proto_subscriber_v0);

            REQUIRE_NOTHROW(actual_peer = subp->get_peer());

            REQUIRE(actual_peer == proto_publisher);
            REQUIRE(actual_peer == proto_publisher_v0);
        }

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

        REQUIRE_NOTHROW(pubp = make_unique<latest_pub_socket_fixture>());
        REQUIRE_NOTHROW(subp = make_unique<latest_sub_socket_fixture>());

        /* Most applications will usually have the pub listen and sub dial. However, this
        creates a problem for our tests, since we can wind up trying to push data before
        the pipe is fully registered due to the accept running asynchronously. */

        REQUIRE_NOTHROW(subp->listen(test_addr));
        REQUIRE_NOTHROW(pubp->dial(test_addr));
        SLEEP_FOR(20ms); // Time for connecting threads.

        SECTION("Subscriber can subscribe") {

            REQUIRE_NOTHROW(subp->set_option(O::sub_subscribe, abc));
            REQUIRE_NOTHROW(subp->set_option(O::sub_subscribe, __empty));

            SECTION("Unsubscribe works") {

                REQUIRE_NOTHROW(subp->set_option(O::sub_unsubscribe, abc));
                REQUIRE_NOTHROW(subp->set_option(O::sub_unsubscribe, __empty));

                REQUIRE_THROWS_AS_MATCHING(subp->set_option(O::sub_unsubscribe, __empty), nng_exception, ThrowsNngException(ec_enoent));
                REQUIRE_THROWS_AS_MATCHING(subp->set_option(O::sub_unsubscribe, hello), nng_exception, ThrowsNngException(ec_enoent));
            }
        }

        SECTION("Publisher cannot subscribe") {

            REQUIRE_THROWS_AS_MATCHING(pubp->set_option(O::sub_subscribe, __empty), nng_exception, ThrowsNngException(ec_enotsup));
        }

        SECTION("Subscriber can receive from publisher") {

            REQUIRE_NOTHROW(subp->set_option(O::sub_subscribe, topics::some));
            REQUIRE_NOTHROW(subp->set_option_usec(O::receive_timeout_usec, CAST_DURATION_TO_USEC(90ms).count()));

            REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
            REQUIRE_NOTHROW(*bmp << topics::some_like_it_hot);
            REQUIRE_NOTHROW(pubp->send(bmp.get()));
            REQUIRE_NOTHROW(subp->try_receive(bmp.get()));
            REQUIRE_THAT(bmp->body()->get(), Equals(topics::some_like_it_hot_buf));

            REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
            REQUIRE_NOTHROW(*bmp << topics::somewhere_over_the_rainbow);
            REQUIRE_THAT(bmp->body()->get(), Equals(topics::somewhere_over_the_rainbow_buf));
            REQUIRE_NOTHROW(pubp->send(bmp.get()));
            REQUIRE_THROWS_AS_MATCHING(subp->try_receive(bmp.get()), nng_exception, ThrowsNngException(ec_etimedout));

            REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
            REQUIRE_NOTHROW(*bmp << topics::some_day_some_how);
            REQUIRE_NOTHROW(pubp->send(bmp.get()));
            REQUIRE_NOTHROW(subp->try_receive(bmp.get()));
            REQUIRE_THAT(bmp->body()->get(), Equals(topics::some_day_some_how_buf));
        }

        SECTION("Subscribers without subsciptions do not receive") {

            REQUIRE_NOTHROW(subp->set_option_usec(O::receive_timeout_usec, CAST_DURATION_TO_USEC(90ms).count()));

            REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
            REQUIRE_NOTHROW(*bmp << topics::some_do_not_like_it);
            REQUIRE_NOTHROW(pubp->send(bmp.get()));
            REQUIRE_THROWS_AS_MATCHING(subp->try_receive(bmp.get()), nng_exception, ThrowsNngException(ec_etimedout));
        }

        SECTION("Subscribers in raw receive") {

            REQUIRE_NOTHROW(subp->set_option_usec(O::receive_timeout_usec, CAST_DURATION_TO_USEC(90ms).count()));
            REQUIRE_NOTHROW(subp->set_option_int(O::raw, 1));

            REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
            REQUIRE_NOTHROW(*bmp << topics::some_like_it_raw);
            REQUIRE_NOTHROW(pubp->send(bmp.get()));
            REQUIRE_NOTHROW(subp->try_receive(bmp.get()));
            REQUIRE_THAT(bmp->body()->get(), Equals(topics::some_like_it_raw_buf));
        }
    }
}