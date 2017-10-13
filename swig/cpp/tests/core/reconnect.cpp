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
    REQUIRE(bmp->has_message() == false);
    REQUIRE(bmp->get_msgp() == nullptr);

    REQUIRE_NOTHROW(bmp->allocate(sz));
    REQUIRE(bmp->has_message() == true);

    REQUIRE(bmp->get_msgp() != nullptr);
    const auto* msgp = bmp->get_msgp();

    REQUIRE(bmp->header() != nullptr);
    REQUIRE(bmp->header()->has_message());
    REQUIRE(bmp->header()->get_msgp() == msgp);

    REQUIRE(bmp->body() != nullptr);
    REQUIRE(bmp->body()->has_message());
    REQUIRE(bmp->body()->get_msgp() == msgp);
}

namespace constants {

    const std::string nng_addr = "inproc://nng";
    const std::string test_addr = "inproc://test";

    const std::string hello = "hello";
    const std::string again = "again";

    const nng::messaging::message_base::buffer_vector_type hello_buf = { 'h','e','l','l','o' };
    const nng::messaging::message_base::buffer_vector_type again_buf = { 'a','g','a','i','n' };
}

/* This is interesting in the event we actually need/want to separate the code under
test with a returning error number. */
#define NNG_TESTS_INIT() int errnum

#define NNG_TESTS_REQUIRE_ERR_EQ(code, expected) errnum = code; \
    REQUIRE(errnum == expected)

/* While I despise macro code expansion, this is "better" for us because in the event
of a legit failure, we see the actual line IN-SITU where the test case ACTUALLY IS. */
#define NNG_TESTS_APPEND_STR(m, s) \
    REQUIRE(::nng_msg_append(m, s, std::strlen(s)) == 0); \
    REQUIRE(::nng_msg_len(m) == std::strlen(s)); \
    REQUIRE(std::memcmp(::nng_msg_body(m), s, std::strlen(s)) == 0)

#define NNG_TESTS_CHECK_STR(m, s) \
    REQUIRE(m != nullptr); \
    REQUIRE(::nng_msg_len(m) == std::strlen(s)); \
    REQUIRE(std::memcmp(::nng_msg_body(m), s, std::strlen(s)) == 0)

//// todo:
//TEST_CASE("Do some math tests", "[math][foo][bar]") {
//
//    SECTION("This is a test") {
//
//        REQUIRE(2 + 2 != 4);
//    }
//
//    SECTION("This is a test 2") {
//
//        REQUIRE(3 + 3 != 6);
//    }
//}

struct c_style_fixture {

    ::nng_socket push;
    ::nng_socket pull;

    c_style_fixture() : push(0), pull(0) {

        REQUIRE(::nng_push_open(&push) == 0);
        REQUIRE(::nng_pull_open(&pull) == 0);

        REQUIRE(push > 0);
        REQUIRE(pull > 0);
    }

    virtual ~c_style_fixture() {
        ::nng_close(push);
        ::nng_close(pull);
        ::nng_fini();
    }
};

#define NNG_TESTS_VERIFY_TEST_CASE_DATA() \
    REQUIRE(std::memcmp(constants::hello.c_str(), "hello", std::strlen("hello")) == 0); \
    REQUIRE(std::memcmp(constants::again.c_str(), "again", std::strlen("again")) == 0); \
    REQUIRE(std::memcmp(constants::nng_addr.c_str(), "inproc://nng", std::strlen("inproc://nng")) == 0); \
    REQUIRE(std::memcmp(constants::test_addr.c_str(), "inproc://test", std::strlen("inproc://test")) == 0)

TEST_CASE("Catch translation of NNG C reconnect unit tests", "[nng][c][reconnect]") {

    using namespace std;
    using namespace std::chrono;
    using namespace constants;

    SECTION("Verify that we have the required data") {
        NNG_TESTS_VERIFY_TEST_CASE_DATA();
    }

    SECTION("We can create a pipeline") {

        /* When the scope rolls back, we want to destroy the fixture, which
        effectively closes the sockets, and finishes the NNG session. */
        c_style_fixture $;

        auto& pull = $.pull;
        auto& push = $.push;

        REQUIRE(::nng_setopt_usec(pull, NNG_OPT_RECONNMINT, 10000) == 0);
        REQUIRE(::nng_setopt_usec(pull, NNG_OPT_RECONNMAXT, 10000) == 0);

        SECTION("Dialing before listening works") {

            // With Reconnect Times configured, Should re-dial until Listener comes online.
            REQUIRE(::nng_dial(push, test_addr.c_str(), nullptr, NNG_FLAG_NONBLOCK) == 0);
            this_thread::sleep_for(100ms);
            REQUIRE(::nng_listen(pull, test_addr.c_str(), nullptr, 0) == 0);

            SECTION("We can send a frame") {

                ::nng_msg* msgp = nullptr;

                this_thread::sleep_for(100ms);

                REQUIRE(::nng_msg_alloc(&msgp, 0) == 0);
                REQUIRE(msgp != nullptr);
                NNG_TESTS_APPEND_STR(msgp, hello.c_str());
                REQUIRE(::nng_sendmsg(push, msgp, 0) == 0);

                /* Nullifying the pointer is NOT a mistake. It is part of the zero-copy message ownership
                semantics. Once the message is "sent", the framework effectively assumes ownership. */
                msgp = nullptr;

                // Which Receiving a Message should allocate when we did not provide one.
                REQUIRE(::nng_recvmsg(pull, &msgp, 0) == 0);
                NNG_TESTS_CHECK_STR(msgp, hello.c_str());

                ::nng_msg_free(msgp);
            }
        }

        SECTION("Reconnection works") {

            ::nng_listener l;

            REQUIRE(::nng_dial(push, test_addr.c_str(), nullptr, NNG_FLAG_NONBLOCK) == 0);
            REQUIRE(::nng_listen(pull, test_addr.c_str(), &l, 0) == 0);
            this_thread::sleep_for(100ms);
            REQUIRE(::nng_listener_close(l) == 0);
            REQUIRE(::nng_listen(pull, test_addr.c_str(), nullptr, 0) == 0);

            SECTION("They still exchange frames") {
                ::nng_msg *msgp;
                ::nng_pipe p1;

                this_thread::sleep_for(100ms);
                REQUIRE(::nng_msg_alloc(&msgp, 0) == 0);
                NNG_TESTS_APPEND_STR(msgp, hello.c_str());
                REQUIRE(::nng_sendmsg(push, msgp, 0) == 0);
                // Ditto message passing ownership semantics.
                msgp = nullptr;
                REQUIRE(::nng_recvmsg(pull, &msgp, 0) == 0);
                REQUIRE(msgp != nullptr);
                NNG_TESTS_CHECK_STR(msgp, hello.c_str());
                REQUIRE((p1 = ::nng_msg_get_pipe(msgp)) > 0);
                ::nng_msg_free(msgp);

                SECTION("Even after pipe close") {
                    ::nng_pipe p2;

                    REQUIRE(::nng_pipe_close(p1) == 0);
                    this_thread::sleep_for(100ms);
                    REQUIRE(::nng_msg_alloc(&msgp, 0) == 0);
                    NNG_TESTS_APPEND_STR(msgp, again.c_str());
                    REQUIRE(::nng_sendmsg(push, msgp, 0) == 0);
                    msgp = nullptr;
                    REQUIRE(::nng_recvmsg(pull, &msgp, 0) == 0);
                    REQUIRE(msgp != nullptr);
                    NNG_TESTS_CHECK_STR(msgp, again.c_str());
                    REQUIRE((p2 = nng_msg_get_pipe(msgp)) > 0);
                    ::nng_msg_free(msgp);
                    REQUIRE(p2 != p1);
                }
            }
        }
    }
}

TEST_CASE("NNG C++ wrapper reconnect works", "[reconnect][cxx][wrapper]") {

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

    SECTION("We can create a pipeline") {

        std::shared_ptr<latest_push_socket> push;
        std::shared_ptr<latest_pull_socket> pull;

        REQUIRE_NOTHROW(push = _session_.create_push_socket());
        REQUIRE_NOTHROW(pull = _session_.create_pull_socket());

        const duration<uint64_t, milli> reconnect_time = 10ms;

        // TODO: TBD: count() might be the way to go here, but I cannot say for sure.
        // http://en.cppreference.com/w/cpp/chrono/duration
        REQUIRE_NOTHROW(pull->set_option_usec(_opt_::min_reconnect_time_usec, reconnect_time.count()));
        REQUIRE_NOTHROW(pull->set_option_usec(_opt_::max_reconnect_time_usec, reconnect_time.count()));

        SECTION("Dialing before listening works") {

            REQUIRE_NOTHROW(push->dial(test_addr, to_int(flag_nonblock)));
            this_thread::sleep_for(100ms);

            REQUIRE_NOTHROW(pull->listen(test_addr));

            /* 'Frame' in this case is loosely speaking. We do not care about exposing the NNG msg structure,
            per se. Rather, we should simply be able to "frame" buffers or strings, accordingly. */
            SECTION("We can send a frame") {

                this_thread::sleep_for(100ms);

                binary_message bm;

                allocate(&bm);

                // And with a little C++ operator overloading help:
                REQUIRE_NOTHROW(bm << hello);

                REQUIRE_THAT(bm.body()->get(), Equals(hello_buf));

                push->send(&bm);

                FAIL();
                REQUIRE_NOTHROW(push->send(&bm));
                REQUIRE_NOTHROW(pull->try_receive(&bm));

                std::string _pulled_actual;

                // Ditto C++ operator overloading.
                _pulled_actual << bm;

                REQUIRE_THAT(_pulled_actual, Equals(hello));
            }
        }

        SECTION("Reconnection works") {

            FAIL();

            REQUIRE_NOTHROW(push->dial(test_addr, to_int(flag_nonblock)));

            {
                shared_ptr<listener> lp;

                REQUIRE_NOTHROW(lp = _session_.create_listener_ep());
                REQUIRE(lp != nullptr);

                REQUIRE_NOTHROW(pull->listen(test_addr, lp.get()));
                this_thread::sleep_for(100ms);

                REQUIRE_NOTHROW(_session_.remove_listener_ep(lp.get()));
                REQUIRE_NOTHROW(lp.reset());
            }

            REQUIRE_NOTHROW(pull->listen(test_addr));

            // TODO: TBD: This was heavily nng_pipe based from reconnect.c
            // TODO: TBD: we may provide comprehension of nng_pipe from a nngcpp POV, but I'm not sure the complexity of nng_msg how that is different from a simple send/receive?
            SECTION("They still exchange frames") {

                FAIL();

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

                    FAIL();

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
