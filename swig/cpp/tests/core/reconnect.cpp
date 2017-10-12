//
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

#include "../tests/messaging/messaging_gymnastics.h"

#include <string>
#include <memory>
#include <thread>


void allocate(nng::messaging::binary_message* const bmp
    , nng::messaging::message_base::size_type sz = 0) {

    REQUIRE(bmp != nullptr);

    REQUIRE_NOTHROW(bmp->allocate(sz));

    REQUIRE(bmp->has_message());

    REQUIRE(bmp->header());
    REQUIRE(bmp->header()->has_message());

    REQUIRE(bmp->body());
    REQUIRE(bmp->body()->has_message());
}

namespace constants {
    const std::string hello = "hello";
    const std::string again = "again";
}

TEST_CASE("Reconnect works", "[reconnect]") {

    using namespace std;
    using namespace std::chrono;
    using namespace nng;
    using namespace nng::protocol;
    using namespace nng::messaging;
    using namespace constants;
    using namespace Catch::Matchers;
    using _opt_ = option_names;

    // Which "at-exit" destructor handles cleaning up the NNG resources: i.e. ::nng_fini.
    session _session_;

    const string addr = "inproc://test";

    SECTION("We can create a pipeline") {

        std::shared_ptr<latest_push_socket> push;
        std::shared_ptr<latest_pull_socket> pull;

        REQUIRE_NOTHROW(push = _session_.create_push_socket());
        REQUIRE_NOTHROW(pull = _session_.create_pull_socket());

        duration<uint64_t, micro> reconnect_time = 10000us;

        // TODO: TBD: count() might be the way to go here, but I cannot say for sure.
        // http://en.cppreference.com/w/cpp/chrono/duration
        REQUIRE_NOTHROW(pull->set_option_usec(_opt_::min_reconnect_time_usec, reconnect_time.count()));
        REQUIRE_NOTHROW(pull->set_option_usec(_opt_::max_reconnect_time_usec, reconnect_time.count()));

        SECTION("Dialing before listening works") {

            REQUIRE_NOTHROW(push->dial(addr, to_int(flag_nonblock)));
            this_thread::sleep_for(100ms);

            REQUIRE_NOTHROW(pull->listen(addr));

            /* 'Frame' in this case is loosely speaking. We do not care about exposing the NNG msg structure,
            per se. Rather, we should simply be able to "frame" buffers or strings, accordingly. */
            SECTION("We can send a frame") {

                std::unique_ptr<binary_message> bmp;

                this_thread::sleep_for(100ms);

                REQUIRE_NOTHROW(bmp = std::make_unique<binary_message>());
                REQUIRE(bmp != nullptr);
                allocate(bmp.get());

                // And with a little C++ operator overloading help:
                REQUIRE_NOTHROW(*bmp << hello);

                REQUIRE_NOTHROW(push->send(bmp.get()));
                FAIL();
                REQUIRE_NOTHROW(pull->try_receive(bmp.get()));

                std::string _pulled_actual;

                // Ditto C++ operator overloading.
                _pulled_actual << *bmp;

                REQUIRE_THAT(_pulled_actual, Equals(hello));
            }
        }

        SECTION("Reconnection works") {

            REQUIRE_NOTHROW(push->dial(addr, to_int(flag_nonblock)));

            {
                shared_ptr<listener> lp;

                REQUIRE_NOTHROW(lp = _session_.create_listener_ep());
                REQUIRE(lp != nullptr);

                REQUIRE_NOTHROW(pull->listen(addr, lp.get()));
                this_thread::sleep_for(100ms);

                REQUIRE_NOTHROW(_session_.remove_listener_ep(lp.get()));
                REQUIRE_NOTHROW(lp.reset());
            }

            REQUIRE_NOTHROW(pull->listen(addr));

            // TODO: TBD: This was heavily nng_pipe based from reconnect.c
            // TODO: TBD: we may provide comprehension of nng_pipe from a nngcpp POV, but I'm not sure the complexity of nng_msg how that is different from a simple send/receive?
            SECTION("They still exchange frames") {

                std::unique_ptr<binary_message> bmp;

                this_thread::sleep_for(100ms);

                REQUIRE_NOTHROW(bmp = std::make_unique<binary_message>());
                REQUIRE(bmp != nullptr);

                allocate(bmp.get());

                *bmp << hello;

                REQUIRE_NOTHROW(push->send(bmp.get()));

                /* The C-based tests simple "reset" the pointer to NULL. Clearly a memory leak in the making.
                Instead, we will make the effort to be friendly to our memory consumption. */
                REQUIRE_NOTHROW(bmp = std::make_unique<binary_message>());
                REQUIRE(bmp != nullptr);

                REQUIRE_NOTHROW(pull->try_receive(bmp.get()));

                REQUIRE(bmp->has_message() == true);
                REQUIRE(bmp->header()->has_message() == true);
                REQUIRE(bmp->body()->has_message() == true);

                // Type conversion magic from the Message to a std::string.
                std::string _gotten_actual = *bmp;

                REQUIRE_THAT(_gotten_actual, Equals(hello));

                std::unique_ptr<message_pipe> mpp1;

                REQUIRE_NOTHROW(mpp1 = std::make_unique<message_pipe>(bmp.get()));
                REQUIRE(mpp1 != nullptr);

                // Now both we and the original C-based unit test agree, reset the memory.
                REQUIRE_NOTHROW(bmp.reset());
                REQUIRE(bmp == nullptr);

                SECTION("Even after pipe close") {

                    std::unique_ptr<message_pipe> mpp2;

                    REQUIRE_NOTHROW(mpp1->close());

                    this_thread::sleep_for(100ms);

                    allocate(bmp.get());

                    *bmp << again;

                    REQUIRE_NOTHROW(push->send(bmp.get()));

                    // Ditto C-based NULL-ifying the pointers without freeing them.
                    REQUIRE_NOTHROW(bmp = std::make_unique<binary_message>());
                    REQUIRE(bmp != nullptr);

                    REQUIRE_NOTHROW(pull->try_receive(bmp.get()));

                    REQUIRE(bmp->has_message() == true);
                    REQUIRE(bmp->header()->has_message() == true);
                    REQUIRE(bmp->body()->has_message() == true);

                    // Type conversion magic from the Message to a std::string.
                    _gotten_actual = *bmp;

                    REQUIRE_THAT(_gotten_actual, Equals(again));

                    auto mp2 = message_pipe(bmp.get());

                    // TODO: TBD: so, we can define the != operator alone, but there is no std::not_equal_to implemented. is there a patch? or migration path from 2015 -> 2017?
                    REQUIRE((*mpp1.get()) != mp2);
                }
            }
        }
    }
}
