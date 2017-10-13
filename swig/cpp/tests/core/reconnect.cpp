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

#define NNGCPP_TESTS_ALLOCATE_MSG_SZ(m, sz) \
    REQUIRE(m.has_message() == false); \
    REQUIRE(m.get_msgp() == nullptr); \
    \
    REQUIRE_NOTHROW(m.allocate(sz)); \
    REQUIRE(m.has_message() == true); \
    REQUIRE(m.get_msgp() != nullptr); \
    \
    REQUIRE(m.header() != nullptr); \
    REQUIRE(m.header()->has_message()); \
    REQUIRE(m.header()->get_msgp() == m.get_msgp()); \
    \
    REQUIRE(m.body() != nullptr); \
    REQUIRE(m.body()->has_message()); \
    REQUIRE(m.body()->get_msgp() == m.get_msgp());

#define NNGCPP_TESTS_ALLOCATE_MSG(m) NNGCPP_TESTS_ALLOCATE_MSG_SZ(m, 0)

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

namespace nng {
    namespace messaging {

        struct message_pipe_fixture : public message_pipe {

            message_pipe_fixture(const message_base* const mbp) : message_pipe(mbp) {
            }

            ::nng_pipe get_pid() const {
                return pid;
            }
        };

        struct binary_message_fixture : public binary_message {

            binary_message_fixture() : binary_message() {
            }

            binary_message_fixture(::nng_msg* msgp) : binary_message(msgp) {
            }

            virtual void free() override {
                binary_message::free();
            }
        };
    }
}

#define NNG_TESTS_VERIFY_TEST_CASE_DATA() \
    REQUIRE_THAT(constants::hello, Catch::Matchers::Equals("hello")); \
    REQUIRE_THAT(constants::again, Catch::Matchers::Equals("again")); \
    REQUIRE_THAT(constants::nng_addr, Catch::Matchers::Equals("inproc://nng")); \
    REQUIRE_THAT(constants::test_addr, Catch::Matchers::Equals("inproc://test"))

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

    // TODO: not sure if I like having "session" part of these unit tests, per se. I almost want session to be the subject of its own set of unit tests (it should be, anyway).
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

            REQUIRE_NOTHROW(push->dial(test_addr, flag_nonblock));
            this_thread::sleep_for(100ms);
            REQUIRE_NOTHROW(pull->listen(test_addr));

            /* 'Frame' in this case is loosely speaking. We do not care about exposing the NNG msg structure,
            per se. Rather, we should simply be able to "frame" buffers or strings, accordingly. */
            SECTION("We can send a frame") {

                binary_message_fixture bm;
                this_thread::sleep_for(100ms);
                NNGCPP_TESTS_ALLOCATE_MSG(bm);
                // And with a little C++ operator overloading help:
                REQUIRE_NOTHROW(bm << hello);
                // TODO: TBD: this sort of testing deserves a dedicated unit test so that we aren't polluting integration scenarios such as this quite as much
                REQUIRE_THAT(bm.body()->get(), Equals(hello_buf));
                REQUIRE_NOTHROW(push->send(&bm));
                /* Ditto message passing semantics. The Send() operation effectively
                nullifies the internal message. */
                REQUIRE(bm.has_message() == false);

                // TODO: TBD: message ownership semantics really deserves a unit test all its own...
                REQUIRE_NOTHROW(pull->try_receive(&bm));
                REQUIRE(bm.has_message() == true);
                // Just verify that the message matches the buffer.
                REQUIRE_THAT(bm.body()->get(), Equals(hello_buf));
            }
        }

        SECTION("Reconnection works") {

            REQUIRE_NOTHROW(push->dial(test_addr, flag_nonblock));

            {
                // We do not need to see the Listener beyond this block.
                auto lp = std::make_unique<listener>();
                REQUIRE_NOTHROW(pull->listen(test_addr, lp.get()));
                this_thread::sleep_for(100ms);
                // No need to burden "session" with this one, either.
                REQUIRE_NOTHROW(lp.reset());
            }

            // Then re-listen to the address.
            REQUIRE_NOTHROW(pull->listen(test_addr));

            // TODO: TBD: This was heavily nng_pipe based from reconnect.c
            // TODO: TBD: we may provide comprehension of nng_pipe from a nngcpp POV, but I'm not sure the complexity of nng_msg how that is different from a simple send/receive?
            SECTION("They still exchange frames") {

                binary_message_fixture bm;

                this_thread::sleep_for(100ms);
                NNGCPP_TESTS_ALLOCATE_MSG(bm);
                REQUIRE_NOTHROW(bm << hello);
                REQUIRE_NOTHROW(push->send(&bm));
                REQUIRE(bm.has_message() == false);

                // See notes above. Sending transfers ownership of the internal message to NNG.
                REQUIRE_NOTHROW(pull->try_receive(&bm));
                REQUIRE(bm.has_message() == true);
                // Just verify that the message matches the buffer.
                REQUIRE_THAT(bm.body()->get(), Equals(hello_buf));

                // TODO: TBD: this one deserves its own unit test as well so that we are not cluttering the integration tests
                message_pipe_fixture mp1(&bm);
                REQUIRE(mp1.is_valid() == true);
                // TODO: TBD: deserves its own unit testing...
                REQUIRE(mp1.get_pid() > 0);
                // We would not normally want to free under any circumstances, except here it is useful to fixture it.
                REQUIRE_NOTHROW(bm.free());
                REQUIRE(bm.has_message() == false);

                SECTION("Even after pipe close") {

                    // Get the fixtured PID for test purposes prior to closing.
                    auto mp1_pid = mp1.get_pid();
                    REQUIRE_NOTHROW(mp1.close());
                    REQUIRE(mp1.is_valid() == false);

                    this_thread::sleep_for(100ms);
                    NNGCPP_TESTS_ALLOCATE_MSG(bm);
                    REQUIRE_NOTHROW(bm << again);
                    // TODO: TBD: send/no-message -> receive/message is a pattern that deserves its own focused unit test...
                    REQUIRE_NOTHROW(push->send(&bm));
                    REQUIRE(bm.has_message() == false);

                    REQUIRE_NOTHROW(pull->try_receive(&bm));
                    REQUIRE(bm.has_message() == true);
                    // Just verify that the message matches the buffer.
                    REQUIRE_THAT(bm.body()->get(), Equals(again_buf));

                    message_pipe_fixture mp2(&bm);
                    REQUIRE(mp2.is_valid() == true);
                    REQUIRE_NOTHROW(bm.free());
                    REQUIRE(mp2.get_pid() != mp1_pid);
                }
            }
        }
    }
}
