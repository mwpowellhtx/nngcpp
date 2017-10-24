//
// Copyright 2017 Michael W Powel <mwpowellhtx@gmail.com>
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

#include <sstream>

// TODO: TBD: now this one is crashing; revisit this one later after sorting out the include situation
namespace nng {
    namespace transactions {

        // TODO: TBD: this one potentially has broader test application, but we'll see...
        struct transaction {
        private:

            int _current_port;

            static std::unique_ptr<transaction> _xactp;

        public:

            transaction() : _current_port(TEST_PORT) {
            }

            static transaction& get_xact() {
                if (_xactp == nullptr) {
                    _xactp = std::make_unique<transaction>();
                }
                return *_xactp;
            }

            const std::string next_addr(const std::string& base_addr) {
                std::ostringstream os;
                os << base_addr << _current_port++;
                return os.str();
            }
        };

        std::unique_ptr<transaction> transaction::_xactp;
    }
}

namespace nng {

    struct c_style_pair_fixture : nng::basic_fixture {

        ::nng_socket s1;
        ::nng_socket c1;
        ::nng_socket c2;

        c_style_pair_fixture() : s1(0), c1(0), c2(0) {
            REQUIRE(::nng_pair1_open(&s1) == 0);
            REQUIRE(::nng_pair1_open(&c1) == 0);
            REQUIRE(::nng_pair1_open(&c2) == 0);
        }

        virtual ~c_style_pair_fixture() {
            // The sockets can be closed here, but they may also have been closed beforehand.
            if (s1) { CHECK_NOFAIL(::nng_close(s1) == 0); }
            if (c1) { CHECK_NOFAIL(::nng_close(c1) == 0); }
            if (c2) { CHECK_NOFAIL(::nng_close(c2) == 0); }
        }
    };
}

namespace constants {

    const std::string pairv1_addr_base = "inproc://pairv1/";

    const std::string alpha = "alpha";
    const std::string beta = "beta";
    const std::string epsilon = "epsilon";
    const std::string gamma = "gamma";
    const std::string one = "one";
    const std::string two = "two";
    const std::string uno = "uno";
    const std::string dos = "dos";
    const std::string ein = "ein";
    const std::string yes = "yes";
    const std::string again = "again";

    const auto alpha_buf = to_buffer(alpha);
    const auto beta_buf = to_buffer(beta);
    const auto epsilon_buf = to_buffer(epsilon);
    const auto gamma_buf = to_buffer(gamma);
    const auto one_buf = to_buffer(one);
    const auto two_buf = to_buffer(two);
    const auto uno_buf = to_buffer(uno);
    const auto dos_buf = to_buffer(dos);
    const auto ein_buf = to_buffer(ein);
    const auto yes_buf = to_buffer(yes);
    const auto again_buf = to_buffer(again);

    const uint32_t feedface = 0xfeedface;
    const uint32_t deadzeros = 0xdead0000;
}

TEST_CASE("Pair v1 protocol works using C style", Catch::Tags("pair"
    , "v1", "protocol", "sockets", "pattern", "nng", "c").c_str()) {

    // TODO: TBD: this test case is not intended as a full treatment of the C-style unit tests, but rather as a comparison for troublesome sections.
    using namespace std;
    using namespace std::chrono;
    using namespace nng;
    using namespace nng::transactions;
    using namespace constants;

    c_style_pair_fixture fixture;

    const auto& s1 = fixture.s1;
    auto& c1 = fixture.c1;
    const auto& c2 = fixture.c2;

    int val;
    ::nng_msg* msg;
    ::nng_pipe p1, p2;

    SECTION("Given a few sockets") {
        const auto addr = transaction::get_xact().next_addr(pairv1_addr_base);

        duration_type const receive_timeout = 300ms;
        duration_type::rep timeout;

        REQUIRE(::nng_setopt_ms(s1, NNG_OPT_RECVTIMEO, receive_timeout.count()) == 0);
        REQUIRE(::nng_setopt_ms(c1, NNG_OPT_RECVTIMEO, receive_timeout.count()) == 0);
        REQUIRE(::nng_setopt_ms(c2, NNG_OPT_RECVTIMEO, receive_timeout.count()) == 0);

        REQUIRE(::nng_getopt_ms(s1, NNG_OPT_RECVTIMEO, &timeout) == 0);
        REQUIRE(timeout == receive_timeout.count());

        // ...

        SECTION("Monogamous mode exerts backpressure") {

            duration_type const timeout = 30ms;

            REQUIRE(::nng_setopt_int(s1, NNG_OPT_RECVBUF, 1) == 0);
            REQUIRE(::nng_setopt_int(s1, NNG_OPT_SENDBUF, 1) == 0);
            REQUIRE(::nng_setopt_int(c1, NNG_OPT_RECVBUF, 1) == 0);
            REQUIRE(::nng_setopt_ms(s1, NNG_OPT_SENDTIMEO, timeout.count()) == 0);

            REQUIRE(::nng_listen(s1, addr.c_str(), NULL, 0) == 0);
            REQUIRE(::nng_dial(c1, addr.c_str(), NULL, 0) == 0);
            SLEEP_FOR(20ms);

            int i, rv;

            // We choose to allow some buffering.  In reality the
            // buffer size is just 1, and we will fail after 2.

            for (i = 0, rv = 0; i < 10; i++) {
                REQUIRE(::nng_msg_alloc(&msg, 0) == 0);
                if ((rv = ::nng_sendmsg(s1, msg, 0)) != 0) {
                    ::nng_msg_free(msg);
                    break;
                }
            }
            CHECK(i < 10);
            REQUIRE(rv == NNG_ETIMEDOUT);
        }

        SECTION("Monogamous raw mode works") {

            // ...

            SECTION("Polyamorous cooked mode works") {

                REQUIRE(::nng_getopt_int(s1, NNG_OPT_PAIR1_POLY, &val) == 0);
                REQUIRE(val == 0);

                REQUIRE(::nng_setopt_int(s1, NNG_OPT_PAIR1_POLY, 1) == 0);
                REQUIRE(::nng_getopt_int(s1, NNG_OPT_PAIR1_POLY, &val) == 0);
                REQUIRE(val == 1);

                REQUIRE(::nng_listen(s1, addr.c_str(), NULL, 0) == 0);
                REQUIRE(::nng_dial(c1, addr.c_str(), NULL, 0) == 0);
                REQUIRE(::nng_dial(c2, addr.c_str(), NULL, 0) == 0);
                SLEEP_FOR(20ms);

                REQUIRE(::nng_msg_alloc(&msg, 0) == 0);
                NNG_TESTS_APPEND_STR(msg, one.c_str());
                REQUIRE(::nng_sendmsg(c1, msg, 0) == 0);
                REQUIRE(::nng_recvmsg(s1, &msg, 0) == 0);
                NNG_TESTS_CHECK_STR(msg, one.c_str());
                REQUIRE((p1 = ::nng_msg_get_pipe(msg)) != 0);
                ::nng_msg_free(msg);

                REQUIRE(::nng_msg_alloc(&msg, 0) == 0);
                NNG_TESTS_APPEND_STR(msg, two.c_str());
                REQUIRE(::nng_sendmsg(c2, msg, 0) == 0);
                REQUIRE(::nng_recvmsg(s1, &msg, 0) == 0);
                NNG_TESTS_CHECK_STR(msg, two.c_str());
                REQUIRE((p2 = nng_msg_get_pipe(msg)) != 0);
                ::nng_msg_free(msg);

                REQUIRE(p1 != p2);

                REQUIRE(::nng_msg_alloc(&msg, 0) == 0);
                ::nng_msg_set_pipe(msg, p1);
                NNG_TESTS_APPEND_STR(msg, uno.c_str());
                REQUIRE(::nng_sendmsg(s1, msg, 0) == 0);
                REQUIRE(::nng_recvmsg(c1, &msg, 0) == 0);
                NNG_TESTS_CHECK_STR(msg, uno.c_str());
                ::nng_msg_free(msg);

                REQUIRE(::nng_msg_alloc(&msg, 0) == 0);
                ::nng_msg_set_pipe(msg, p2);
                NNG_TESTS_APPEND_STR(msg, dos.c_str());
                REQUIRE(::nng_sendmsg(s1, msg, 0) == 0);
                REQUIRE(::nng_recvmsg(c2, &msg, 0) == 0);
                NNG_TESTS_CHECK_STR(msg, dos.c_str());
                ::nng_msg_free(msg);

                // This one is not a firm requirement, but still may want to investigate ENOENT.
                REQUIRE(::nng_close(c1) == 0);

                REQUIRE(::nng_msg_alloc(&msg, 0) == 0);
                ::nng_msg_set_pipe(msg, p1);
                NNG_TESTS_APPEND_STR(msg, ein.c_str());
                REQUIRE(::nng_sendmsg(s1, msg, 0) == 0);
                REQUIRE(::nng_recvmsg(c2, &msg, 0) == NNG_ETIMEDOUT);
            }

            SECTION("Polyamorous default works") {

                REQUIRE(::nng_setopt_int(s1, NNG_OPT_PAIR1_POLY, 1) == 0);

                REQUIRE(::nng_listen(s1, addr.c_str(), NULL, 0) == 0);
                REQUIRE(::nng_dial(c1, addr.c_str(), NULL, 0) == 0);
                SLEEP_FOR(100ms);
                REQUIRE(::nng_dial(c2, addr.c_str(), NULL, 0) == 0);
                SLEEP_FOR(20ms);

                REQUIRE(::nng_msg_alloc(&msg, 0) == 0);
                NNG_TESTS_APPEND_STR(msg, yes.c_str());
                REQUIRE(::nng_sendmsg(s1, msg, 0) == 0);
                REQUIRE(::nng_recvmsg(c1, &msg, 0) == 0);
                NNG_TESTS_CHECK_STR(msg, yes.c_str());
                ::nng_msg_free(msg);

                /* Error conditions are "acceptable" under these circumstances,
                but I still might like to know what happened with ENOENT. */
                REQUIRE(::nng_close(c1) == 0);
                SLEEP_FOR(10ms);

                REQUIRE(::nng_msg_alloc(&msg, 0) == 0);
                NNG_TESTS_APPEND_STR(msg, again.c_str());
                REQUIRE(::nng_sendmsg(s1, msg, 0) == 0);
                REQUIRE(::nng_recvmsg(c2, &msg, 0) == 0);
                NNG_TESTS_CHECK_STR(msg, again.c_str());
                ::nng_msg_free(msg);
            }
        }
    }
}

TEST_CASE("Pair v1 protocol works using C++ wrapper", Catch::Tags("pair"
    , "v1", "protocol", "sockets", "pattern", "nng", "cxx").c_str()) {

    using namespace std;
    using namespace std::chrono;
    using namespace nng;
    using namespace nng::protocol;
    using namespace nng::messaging;
    using namespace nng::exceptions;
    using namespace nng::transactions;
    using namespace constants;
    using namespace Catch::Matchers;
    using O = option_names;

    uint32_t hops;
    uint32_t actual_api;
    int32_t actual_opt;
    duration_type timeout_duration;

    basic_fixture fixture;

    SECTION("Given a few sockets") {

        int ttl;

        const auto addr = transaction::get_xact().next_addr(pairv1_addr_base);

        unique_ptr<latest_pair_socket> serverp1;
        unique_ptr<latest_pair_socket> clientp1;
        unique_ptr<latest_pair_socket> clientp2;

        REQUIRE_NOTHROW(serverp1 = make_unique<latest_pair_socket>());
        REQUIRE_NOTHROW(clientp1 = make_unique<latest_pair_socket>());
        REQUIRE_NOTHROW(clientp2 = make_unique<latest_pair_socket>());

        const auto receive_timeout = 300ms;

        REQUIRE_NOTHROW(serverp1->options()->set(O::recv_timeout_duration, receive_timeout));
        REQUIRE_NOTHROW(clientp1->options()->set(O::recv_timeout_duration, receive_timeout));
        REQUIRE_NOTHROW(clientp2->options()->set(O::recv_timeout_duration, receive_timeout));

        REQUIRE_NOTHROW(serverp1->options()->get(O::recv_timeout_duration, timeout_duration));
        // TODO: TBD: looking forward to full Catch v2; including CHRONO comprehension.
        REQUIRE(timeout_duration == 300ms);

        unique_ptr<binary_message> bmp;
        unique_ptr<message_pipe> mpp1, mpp2;

        SECTION("Monogamous cooked mode works") {

            REQUIRE_NOTHROW(serverp1->listen(addr));
            REQUIRE_NOTHROW(clientp1->dial(addr));
            SLEEP_FOR(20ms);

            REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
            REQUIRE_NOTHROW(*bmp << alpha);
            REQUIRE_NOTHROW(clientp1->send(bmp.get()));
            REQUIRE_NOTHROW(serverp1->try_receive(bmp.get()));
            REQUIRE_THAT(bmp->body()->get(), Equals(alpha_buf));

            REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
            REQUIRE_NOTHROW(*bmp << beta);
            REQUIRE_NOTHROW(serverp1->send(bmp.get()));
            REQUIRE_NOTHROW(clientp1->try_receive(bmp.get()));
            REQUIRE_THAT(bmp->body()->get(), Equals(beta_buf));
        }

        SECTION("Monogamous mode ignores new connections") {

            REQUIRE_NOTHROW(serverp1->listen(addr));
            REQUIRE_NOTHROW(clientp1->dial(addr));
            SLEEP_FOR(100ms);
            REQUIRE_NOTHROW(clientp2->dial(addr));

            REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
            REQUIRE_NOTHROW(*bmp << one);
            REQUIRE_NOTHROW(clientp1->send(bmp.get()));
            REQUIRE_NOTHROW(serverp1->try_receive(bmp.get()));
            REQUIRE_THAT(bmp->body()->get(), Equals(one_buf));

            REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
            REQUIRE_NOTHROW(*bmp << two);
            REQUIRE_NOTHROW(clientp2->send(bmp.get()));
            REQUIRE_THROWS_AS_MATCHING(serverp1->try_receive(bmp.get()), nng_exception, THROWS_NNG_EXCEPTION(ec_etimedout));
        }

        SECTION("Cannot set raw mode after connected") {

            REQUIRE_NOTHROW(serverp1->listen(addr));
            REQUIRE_NOTHROW(clientp1->dial(addr));
            SLEEP_FOR(100ms);

            REQUIRE_THROWS_AS_MATCHING(serverp1->options()->set_int(O::raw, 1), nng_exception, THROWS_NNG_EXCEPTION(ec_estate));
            REQUIRE_THROWS_AS_MATCHING(clientp1->options()->set_int(O::raw, 1), nng_exception, THROWS_NNG_EXCEPTION(ec_estate));
        }

        SECTION("Polyamorous mode is best effort") {

            REQUIRE_NOTHROW(serverp1->options()->set_int(O::pair1_polyamorous, 1));
            REQUIRE_NOTHROW(serverp1->options()->set_int(O::recv_buf, 1));
            REQUIRE_NOTHROW(serverp1->options()->set_int(O::send_buf, 1));
            REQUIRE_NOTHROW(clientp1->options()->set_int(O::recv_buf, 1));
            REQUIRE_NOTHROW(clientp1->options()->set_int(O::recv_buf, 1));
            REQUIRE_NOTHROW(serverp1->options()->set(O::send_timeout_duration, 100ms));

            REQUIRE_NOTHROW(serverp1->listen(addr));
            REQUIRE_NOTHROW(clientp1->dial(addr));
            SLEEP_FOR(20ms);

            int i;

            for (i = 0; i < 10; i++) {
                REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
                try {
                    CHECK_NOTHROW(serverp1->send(bmp.get()));
                }
                catch (...) {
                    break;
                }
            }

            REQUIRE(i == 10);
        }

        SECTION("Monogamous mode exerts backpressure") {

            REQUIRE_NOTHROW(serverp1->options()->set_int(O::recv_buf, 1));
            REQUIRE_NOTHROW(serverp1->options()->set_int(O::send_buf, 1));
            REQUIRE_NOTHROW(clientp1->options()->set_int(O::recv_buf, 1));
            REQUIRE_NOTHROW(serverp1->options()->set(O::send_timeout_duration, 30ms));

            REQUIRE_NOTHROW(serverp1->listen(addr));
            REQUIRE_NOTHROW(clientp1->dial(addr));
            SLEEP_FOR(20ms);

            /* We choose to allow some buffering. In reality the buffer size
            is just one (1), and we will fail after two (2). */

            int i;
            error_code_type actual_ec;

            // Assume successful up front. If no exception, then groovy.
            for (i = 0; i < 10; actual_ec = ec_enone, i++) {
                REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
                REQUIRE_NOTHROW(bmp->allocate());
                try {
                    // This is a RARE instance where we do NOT want to check/require ANYTHING.
                    serverp1->send(bmp.get());
                }
                catch (nng_exception& ex) {
                    CHECK_NOTHROW(bmp.reset());
                    actual_ec = ex.error_code;
                    break;
                }
            }

            CHECK(i < 10);
            REQUIRE(actual_ec == ec_etimedout);
        }

        SECTION("Cannot set polyamorous mode after connect") {

            REQUIRE_NOTHROW(serverp1->listen(addr));
            REQUIRE_NOTHROW(clientp1->dial(addr));
            SLEEP_FOR(100ms);

            REQUIRE_THROWS_AS_MATCHING(serverp1->options()->set_int(O::pair1_polyamorous, 1), nng_exception, THROWS_NNG_EXCEPTION(ec_estate));
        }

        SECTION("Monogamous raw mode works") {

            REQUIRE_NOTHROW(serverp1->options()->set_int(O::raw, 1));
            REQUIRE_NOTHROW(clientp1->options()->set_int(O::raw, 1));
            REQUIRE_NOTHROW(clientp2->options()->set_int(O::raw, 1));

            REQUIRE_NOTHROW(serverp1->listen(addr));
            REQUIRE_NOTHROW(clientp1->dial(addr));
            SLEEP_FOR(20ms);

            SECTION("Send and receive work") {

                REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
                REQUIRE_NOTHROW(*bmp << gamma);
                // TODO: TBD: so, is this a number of hops?
                REQUIRE_NOTHROW(bmp->header()->append(1));
                // TODO: TBD: this strikes me as a header/api unit test not a pair integration test...
                const auto expected_header_sz = sizeof(uint32_t);
                // TODO: TBD: this little check really deserves its own set of message header unit tests...
                REQUIRE_NOTHROW(bmp->header()->get_size() == expected_header_sz);
                REQUIRE_NOTHROW(clientp1->send(bmp.get()));
                REQUIRE_NOTHROW(serverp1->try_receive(bmp.get()));
                // TODO: TBD: I'm not sure what purpose getting the message pipes really serves... again: really deserves a dedicated set of unit tests just for this aspect alone...
                REQUIRE_NOTHROW(mpp1 = make_unique<message_pipe>(bmp.get()));
                REQUIRE_THAT(bmp->body()->get(), Equals(gamma_buf));
                REQUIRE(bmp->header()->get_size() == expected_header_sz);
                // TODO: TBD: ditto header unit tests...
                REQUIRE_NOTHROW(bmp->header()->ltrim(hops));
                REQUIRE(hops == 2);

                REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
                REQUIRE_NOTHROW(*bmp << epsilon);
                // TODO: TBD: so, is this a number of hops?
                REQUIRE_NOTHROW(bmp->header()->append(1));
                REQUIRE_NOTHROW(serverp1->send(bmp.get()));
                REQUIRE_NOTHROW(clientp1->try_receive(bmp.get()));
                REQUIRE_THAT(bmp->body()->get(), Equals(epsilon_buf));
                // TODO: TBD: ditto header unit tests...
                REQUIRE(bmp->header()->get_size() == expected_header_sz);
                REQUIRE_NOTHROW(bmp->header()->ltrim(hops));
                REQUIRE_NOTHROW(mpp2 = make_unique<message_pipe>(bmp.get()));
                REQUIRE(hops == 2);

                // TODO: TBD: and while we're here throw this in for good measure...
                REQUIRE(*mpp1 != *mpp2);
            }

            SECTION("Missing raw header fails") {

                REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
                REQUIRE_NOTHROW(clientp1->send(bmp.get()));
                REQUIRE_THROWS_AS_MATCHING(serverp1->try_receive(bmp.get()), nng_exception, THROWS_NNG_EXCEPTION(ec_etimedout));

                REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
                REQUIRE_NOTHROW(bmp->body()->append(feedface));
                // TODO: TBD: ditto body/header unit tests...
                REQUIRE_NOTHROW(bmp->header()->append(1));
                REQUIRE_NOTHROW(clientp1->send(bmp.get()));
                REQUIRE_NOTHROW(serverp1->try_receive(bmp.get()));
                REQUIRE_NOTHROW(bmp->body()->ltrim(actual_api));
                REQUIRE(actual_api == feedface);
            }

            SECTION("Reserved bits in raw header") {

                // TODO: TBD: non zero bits? there are zeros here?
                SECTION("Nonzero bits fail") {
                    REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
                    REQUIRE_NOTHROW(bmp->header()->append(deadzeros));
                    REQUIRE_NOTHROW(clientp1->send(bmp.get()));
                    REQUIRE_THROWS_AS_MATCHING(serverp1->try_receive(bmp.get()), nng_exception, THROWS_NNG_EXCEPTION(ec_etimedout));
                }

                // TODO: TBD: zero bits? there are non zeros here...
                SECTION("Zero bits pass") {
                    REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
                    REQUIRE_NOTHROW(bmp->body()->append(feedface));
                    REQUIRE_NOTHROW(bmp->header()->append(1));
                    REQUIRE_NOTHROW(clientp1->send(bmp.get()));
                    REQUIRE_NOTHROW(serverp1->try_receive(bmp.get()));
                    REQUIRE_NOTHROW(bmp->body()->ltrim(actual_api));
                    REQUIRE(actual_api == feedface);
                }
            }

            SECTION("TTL is honored") {

                const int max_ttl = 0xff;
                uint32_t initial_ttl = 3;
                int expected = 4;

                ttl = 0;

                // TODO: TBD: ditto options unit tests...
                REQUIRE_NOTHROW(serverp1->options()->set_int(O::max_ttl, expected));
                REQUIRE_NOTHROW(serverp1->options()->get_int(O::max_ttl, ttl));
                REQUIRE(ttl == expected);

                SECTION("Bad TTL bounces") {
                    REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
                    REQUIRE_NOTHROW(bmp->header()->append(expected));
                    REQUIRE_NOTHROW(clientp1->send(bmp.get()));
                    REQUIRE_THROWS_AS_MATCHING(serverp1->try_receive(bmp.get()), nng_exception, THROWS_NNG_EXCEPTION(ec_etimedout));
                }

                SECTION("Good TTL passes") {
                    REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
                    REQUIRE_NOTHROW(bmp->body()->append(feedface));
                    REQUIRE_NOTHROW(bmp->header()->append(initial_ttl));
                    REQUIRE_NOTHROW(clientp1->send(bmp.get()));
                    REQUIRE_NOTHROW(serverp1->try_receive(bmp.get()));
                    REQUIRE_NOTHROW(bmp->body()->ltrim(actual_api));
                    REQUIRE(actual_api == feedface);
                    REQUIRE_NOTHROW(bmp->header()->ltrim(actual_api));
                    // TODO: TBD: ditto header tests; this is looking like a counter?
                    REQUIRE(actual_api == initial_ttl + 1);
                }

                SECTION("Large TTL passes") {
                    REQUIRE_NOTHROW(serverp1->options()->set_int(O::max_ttl, ttl = max_ttl));
                    REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
                    // TODO: TBD: ditto narrowly focused unit tests...
                    const int expected_api = 1234;
                    REQUIRE_NOTHROW(bmp->body()->append(expected_api));
                    // TODO: TBD: yes, TTL-1; plausible unit test candidate for narrow unit tests...
                    REQUIRE_NOTHROW(bmp->header()->append(initial_ttl = ttl - 1));
                    REQUIRE_NOTHROW(clientp1->send(bmp.get()));
                    REQUIRE_NOTHROW(serverp1->try_receive(bmp.get()));
                    REQUIRE_NOTHROW(bmp->body()->ltrim(actual_api));
                    REQUIRE(actual_api == expected_api);
                    REQUIRE_NOTHROW(bmp->header()->ltrim(actual_api));
                    // TODO: TBD: was this the intention? (cryptic though it may have been...)
                    REQUIRE(actual_api == initial_ttl + 1);
                }

                SECTION("Max TTL fails") {
                    REQUIRE_NOTHROW(serverp1->options()->set_int(O::max_ttl, ttl = max_ttl));
                    REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
                    REQUIRE_NOTHROW(bmp->header()->append(ttl));
                    REQUIRE_NOTHROW(clientp1->send(bmp.get()));
                    REQUIRE_THROWS_AS_MATCHING(serverp1->try_receive(bmp.get()), nng_exception, THROWS_NNG_EXCEPTION(ec_etimedout));
                }
            }
        }
        SECTION("We cannot set insane TTLs") {

            // TODO: TBD: really should be part of an options-oriented unit test...
            REQUIRE_THROWS_AS_MATCHING(serverp1->options()->set_int(O::max_ttl, ttl = 0), nng_exception, THROWS_NNG_EXCEPTION(ec_einval));
            REQUIRE_THROWS_AS_MATCHING(serverp1->options()->set_int(O::max_ttl, ttl = 1000), nng_exception, THROWS_NNG_EXCEPTION(ec_einval));
            // TODO: TBD: still a candidate for unit tests.
            // Note the subtle difference in set call.
            REQUIRE_THROWS_AS_MATCHING(serverp1->options()->set(O::max_ttl, &(ttl = 8), 1), nng_exception, THROWS_NNG_EXCEPTION(ec_einval));
        }

        SECTION("Polyamorous cooked mode works") {

            // TODO: TBD: so all these hoops opening up this case need to be part of the narrower scope options unit test
            REQUIRE_NOTHROW(serverp1->options()->get_int(O::pair1_polyamorous, actual_opt));
            REQUIRE(actual_opt == 0);

            REQUIRE_NOTHROW(serverp1->options()->set_int(O::pair1_polyamorous, 1));
            REQUIRE_NOTHROW(serverp1->options()->get_int(O::pair1_polyamorous, actual_opt));
            REQUIRE(actual_opt == 1);

            REQUIRE_NOTHROW(serverp1->listen(addr));
            REQUIRE_NOTHROW(clientp1->dial(addr));
            REQUIRE_NOTHROW(clientp2->dial(addr));
            SLEEP_FOR(20ms);

            REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
            REQUIRE_NOTHROW(*bmp << one);
            REQUIRE_NOTHROW(clientp1->send(bmp.get()));
            REQUIRE_NOTHROW(serverp1->try_receive(bmp.get()));
            REQUIRE_THAT(bmp->body()->get(), Equals(one_buf));
            // TODO: TBD: ditto focused unit tests...
            REQUIRE_NOTHROW(mpp1 = make_unique<message_pipe>(bmp.get()));
            REQUIRE(mpp1->has_one() == true);

            REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
            REQUIRE_NOTHROW(*bmp << two);
            REQUIRE_NOTHROW(clientp2->send(bmp.get()));
            REQUIRE_NOTHROW(serverp1->try_receive(bmp.get()));
            REQUIRE_THAT(bmp->body()->get(), Equals(two_buf));
            REQUIRE_NOTHROW(mpp2 = make_unique<message_pipe>(bmp.get()));
            REQUIRE(mpp2->has_one() == true);
            REQUIRE(*mpp1 != *mpp2);

            // Slightly different from the C-based unit tests. We will need it for the next lines.
            REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
            REQUIRE_NOTHROW(bmp->set_pipe(mpp1.get()));
            REQUIRE_NOTHROW(*bmp << uno);
            REQUIRE_NOTHROW(serverp1->send(bmp.get()));
            REQUIRE_NOTHROW(clientp1->try_receive(bmp.get()));
            REQUIRE_THAT(bmp->body()->get(), Equals(uno_buf));

            // Similar scenario as with Uno.
            REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
            REQUIRE_NOTHROW(bmp->set_pipe(mpp2.get()));
            REQUIRE_NOTHROW(*bmp << dos);
            REQUIRE_NOTHROW(serverp1->send(bmp.get()));
            REQUIRE_NOTHROW(clientp2->try_receive(bmp.get()));
            REQUIRE_THAT(bmp->body()->get(), Equals(dos_buf));

            // Stronger than Close, let's Reset or Destroy the client (which Closes).
            REQUIRE_NOTHROW(clientp1.reset());

            // TODO: TBD: from the "C" code, playing a bit fast and loose with the pipe ownership semantics here aren't we?
            REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
            REQUIRE_NOTHROW(bmp->set_pipe(mpp1.get()));
            REQUIRE_NOTHROW(*bmp << ein);
            REQUIRE_NOTHROW(serverp1->send(bmp.get()));
            REQUIRE_THROWS_AS_MATCHING(clientp2->try_receive(bmp.get()), nng_exception, THROWS_NNG_EXCEPTION(ec_etimedout));
        }

        SECTION("Polyamorous default works") {

            REQUIRE_NOTHROW(serverp1->options()->set_int(O::pair1_polyamorous, 1));

            REQUIRE_NOTHROW(serverp1->listen(addr));
            REQUIRE_NOTHROW(clientp1->dial(addr));
            SLEEP_FOR(100ms);
            REQUIRE_NOTHROW(clientp2->dial(addr));
            SLEEP_FOR(20ms);

            REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
            REQUIRE_NOTHROW(*bmp << yes);
            REQUIRE_NOTHROW(serverp1->send(bmp.get()));
            REQUIRE_NOTHROW(clientp1->try_receive(bmp.get()));
            REQUIRE_THAT(bmp->body()->get(), Equals(yes_buf));

            // Stronger than Close, let's Reset or Destroy the client (which Closes).
            REQUIRE_NOTHROW(clientp1.reset());
            SLEEP_FOR(10ms);

            REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
            REQUIRE_NOTHROW(*bmp << again);
            REQUIRE_NOTHROW(serverp1->send(bmp.get()));
            REQUIRE_NOTHROW(clientp2->try_receive(bmp.get()));
            REQUIRE_THAT(bmp->body()->get(), Equals(again_buf));
        }

        SECTION("Polyamorous raw mode works") {

            // TODO: TBD: here we go again, doing hoop jumping that really belongs in its own unit tests for options...
            REQUIRE_NOTHROW(serverp1->options()->get_int(O::pair1_polyamorous, actual_opt));
            REQUIRE(actual_opt == 0);

            REQUIRE_NOTHROW(serverp1->options()->set_int(O::pair1_polyamorous, 1));
            REQUIRE_NOTHROW(serverp1->options()->get_int(O::pair1_polyamorous, actual_opt));
            REQUIRE(actual_opt == 1);

            // TODO: It probably serves a purpose on one level; but if we test it in its own unit test, we can just set it and be done with it, with confidence.
            actual_opt = 0;
            REQUIRE_NOTHROW(serverp1->options()->set_int(O::raw, 1));
            REQUIRE_NOTHROW(serverp1->options()->get_int(O::raw, actual_opt));
            REQUIRE(actual_opt == 1);

            REQUIRE_NOTHROW(serverp1->listen(addr));
            REQUIRE_NOTHROW(clientp1->dial(addr));
            REQUIRE_NOTHROW(clientp2->dial(addr));
            SLEEP_FOR(20ms);

            SECTION("Send and receive both work") {
                REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
                REQUIRE_NOTHROW(*bmp << one);
                REQUIRE_NOTHROW(clientp1->send(bmp.get()));
                REQUIRE_NOTHROW(serverp1->try_receive(bmp.get()));
                REQUIRE_THAT(bmp->body()->get(), Equals(one_buf));
                REQUIRE_NOTHROW(mpp1 = make_unique<message_pipe>(bmp.get()));
                REQUIRE(mpp1->has_one() == true);
                REQUIRE_NOTHROW(bmp->header()->ltrim(hops));
                REQUIRE(hops == 1);

                REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
                REQUIRE_NOTHROW(*bmp << two);
                REQUIRE_NOTHROW(clientp2->send(bmp.get()));
                REQUIRE_NOTHROW(serverp1->try_receive(bmp.get()));
                REQUIRE_THAT(bmp->body()->get(), Equals(two_buf));
                REQUIRE_NOTHROW(mpp2 = make_unique<message_pipe>(bmp.get()));
                REQUIRE(mpp2->has_one() == true);
                REQUIRE_NOTHROW(bmp->header()->ltrim(hops));
                REQUIRE(hops == 1);
                REQUIRE_NOTHROW(bmp.reset());

                // Doing a similar dance as a couple of SECTION's ago.
                REQUIRE(*mpp1 != *mpp2);

                REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
                REQUIRE_NOTHROW(bmp->set_pipe(mpp1.get()));
                REQUIRE_NOTHROW(*bmp << uno);
                REQUIRE_NOTHROW(bmp->header()->append(1));
                REQUIRE_NOTHROW(serverp1->send(bmp.get()));
                REQUIRE_NOTHROW(clientp1->try_receive(bmp.get()));
                REQUIRE_THAT(bmp->body()->get(), Equals(uno_buf));

                REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
                REQUIRE_NOTHROW(bmp->set_pipe(mpp2.get()));
                REQUIRE_NOTHROW(*bmp << dos);
                REQUIRE_NOTHROW(bmp->header()->append(1));
                REQUIRE_NOTHROW(serverp1->send(bmp.get()));
                REQUIRE_NOTHROW(clientp2->try_receive(bmp.get()));
                REQUIRE_THAT(bmp->body()->get(), Equals(dos_buf));
            }

            SECTION("Closed pipes do not work") {
                REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
                REQUIRE_NOTHROW(*bmp << one);
                REQUIRE_NOTHROW(clientp1->send(bmp.get()));
                REQUIRE_NOTHROW(serverp1->try_receive(bmp.get()));
                REQUIRE_THAT(bmp->body()->get(), Equals(one_buf));
                REQUIRE_NOTHROW(mpp1 = make_unique<message_pipe>(bmp.get()));
                REQUIRE(mpp1->has_one() == true);

                // More hoop jumping.
                REQUIRE_NOTHROW(clientp1->close());

                REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
                REQUIRE_NOTHROW(bmp->set_pipe(mpp1.get()));
                REQUIRE_NOTHROW(*bmp << ein);
                REQUIRE_NOTHROW(bmp->header()->append(1));
                REQUIRE_NOTHROW(serverp1->send(bmp.get()));
                REQUIRE_THROWS_AS_MATCHING(clientp2->try_receive(bmp.get()), nng_exception, THROWS_NNG_EXCEPTION(ec_etimedout));
            }
        }
    }
}
