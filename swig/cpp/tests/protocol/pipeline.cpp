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

#include "../catch/catch_nng_exception_matcher.hpp"
#include "../catch/catch_exception_translations.hpp"
#include "../catch/catch_macros.hpp"
#include "../helpers/chrono.hpp"

#include <nngcpp.h>

#include <string>

// TODO: TBD: this pattern is fairly common, should be a separate include/module...
struct pipeline_fixture {
    virtual ~pipeline_fixture() {
        ::nng_fini();
    }
};

namespace constants {
    const std::string test_addr = "inproc://test";
    const std::string hello = "hello";
    const std::string abc = "abc";
    const std::string def = "def";
    const nng::messaging::message_base::buffer_vector_type hello_buf = { 'h','e','l','l','o' };
    const nng::messaging::message_base::buffer_vector_type abc_buf = { 'a','b','c' };
    const nng::messaging::message_base::buffer_vector_type def_buf = { 'd','e','f' };
}

namespace nng {
    namespace protocol {
        namespace v0 {

            class push_socket_inv_ops_fixture : public push_socket {
            public:

                push_socket_inv_ops_fixture() : push_socket() {
                }

                virtual ~push_socket_inv_ops_fixture() {
                }

                std::unique_ptr<push_socket::binary_message_type> receive(flag_type flags = flag_none) {
                    return push_socket::receive(flags);
                }

                int try_receive(binary_message_type* const bmp, flag_type flags = flag_none) {
                    return push_socket::try_receive(bmp, flags);
                }

                buffer_vector_type receive(size_type& sz, flag_type flags = flag_none) {
                    return push_socket::receive(sz, flags);
                }

                int try_receive(buffer_vector_type* const bufp, size_type& sz, flag_type flags = flag_none) {
                    return push_socket::try_receive(bufp, sz, flags);
                }
            };

            class pull_socket_inv_ops_fixture : public pull_socket {
            public:

                pull_socket_inv_ops_fixture() : pull_socket() {
                }

                virtual ~pull_socket_inv_ops_fixture() {
                }

                virtual void send(binary_message_type* const bmp, flag_type flags = flag_none) override {
                    pull_socket::send(bmp, flags);
                }

                virtual int send(const buffer_vector_type* const bufp, flag_type flags = flag_none) override {
                    return pull_socket::send(bufp, flags);
                }

                virtual int send(const buffer_vector_type* const bufp, size_type sz, flag_type flags = flag_none) override {
                    return pull_socket::send(bufp, sz, flags);
                }
            };
        }

        typedef v0::push_socket_inv_ops_fixture latest_push_socket_inv_ops_fixture;
        typedef v0::pull_socket_inv_ops_fixture latest_pull_socket_inv_ops_fixture;
    }
}

TEST_CASE("Rule out the possibility of invalid operations", "[pipeline][push][pull][invalid][operations][send][receive][try]") {

    using namespace nng;
    using namespace nng::protocol;
    using namespace nng::messaging;
    using namespace trx;

    // Even though we are technically doing nothing with these, we MUST initialize them anyhow.
    socket::size_type sz = 0;
    binary_message* bmp = nullptr;
    message_base::buffer_vector_type* bufp = nullptr;

    SECTION("Push sockets cannot receive messages") {

        latest_push_socket_inv_ops_fixture push;

        REQUIRE_THROWS_AS(push.receive(), invalid_operation);
        REQUIRE_THROWS_AS(push.try_receive(bmp), invalid_operation);
        REQUIRE_THROWS_AS(push.receive(sz), invalid_operation);
        REQUIRE_THROWS_AS(push.try_receive(bufp, sz), invalid_operation);
    }

    SECTION("Pull sockets cannot send messages") {

        latest_pull_socket_inv_ops_fixture pull;

        REQUIRE_THROWS_AS(pull.send(bufp), invalid_operation);
        REQUIRE_THROWS_AS(pull.send(bmp), invalid_operation);
        REQUIRE_THROWS_AS(pull.send(bufp, sz), invalid_operation);
    }
}

TEST_CASE("Pipeline (push/pull) pattern", "[pipeline][push][pull][pattern][protocol][sockets][cxx]") {

    using namespace std;
    using namespace std::chrono;
    using namespace nng;
    using namespace nng::protocol;
    using namespace nng::messaging;
    using namespace trx;
    using namespace Catch;
    using namespace Catch::Matchers;
    using namespace constants;
    using _opt_ = option_names;

    SECTION("We can create a Push socket") {

        unique_ptr<latest_push_socket> pushsp;

        REQUIRE_NOTHROW(pushsp = make_unique<latest_push_socket>());
        REQUIRE(pushsp != nullptr);

        SECTION("Protocols match") {

            const auto actual_proto = pushsp->get_protocol();

            REQUIRE(actual_proto == nng::proto_pusher);
            REQUIRE(actual_proto == nng::proto_pusher_v0);

            const auto actual_peer = pushsp->get_peer();
            REQUIRE(actual_peer == nng::proto_puller);
            REQUIRE(actual_peer == nng::proto_puller_v0);
        }

        SECTION("Can close socket") {
            REQUIRE_NOTHROW(pushsp.reset());
            REQUIRE(pushsp == nullptr);
        }
    }

    SECTION("We can create a Pull socket") {

        unique_ptr<latest_pull_socket> pullsp;

        REQUIRE_NOTHROW(pullsp = make_unique<latest_pull_socket>());
        REQUIRE(pullsp != nullptr);

        SECTION("Protocols match") {

            const auto actual_proto = pullsp->get_protocol();

            REQUIRE(actual_proto == nng::proto_puller);
            REQUIRE(actual_proto == nng::proto_puller_v0);

            const auto actual_peer = pullsp->get_peer();
            REQUIRE(actual_peer == nng::proto_pusher);
            REQUIRE(actual_peer == nng::proto_pusher_v0);
        }

        SECTION("Can close socket") {
            REQUIRE_NOTHROW(pullsp.reset());
            REQUIRE(pullsp == nullptr);
        }
    }
}

TEST_CASE("We can create a linked push/pull pair", "[pipeline][push][pull][linked]") {

    using namespace std;
    using namespace nng::protocol;
    using namespace nng::messaging;
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

        binary_message bm;

        REQUIRE_NOTHROW(bm << hello);
        REQUIRE_NOTHROW(pushsp->send(&bm));
        REQUIRE_NOTHROW(pullsp->try_receive(&bm));
        REQUIRE_THAT(bm.body()->get(), Equals(hello_buf));
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

TEST_CASE("Load balancing works", "[pipeline][push][pull][load][balancing]") {

    using namespace std;
    using namespace std::chrono;
    using namespace nng;
    using namespace nng::protocol;
    using namespace nng::messaging;
    using namespace constants;
    using namespace Catch::Matchers;
    using _opt_ = option_names;

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

    REQUIRE_NOTHROW(pushsp->set_option_int(_opt_::receive_buffer, default_buffer_size));
    REQUIRE_NOTHROW(pullsp1->set_option_int(_opt_::receive_buffer, default_buffer_size));
    REQUIRE_NOTHROW(pullsp2->set_option_int(_opt_::receive_buffer, default_buffer_size));
    REQUIRE_NOTHROW(pullsp3->set_option_int(_opt_::receive_buffer, default_buffer_size));

    REQUIRE_NOTHROW(pushsp->set_option_int(_opt_::send_buffer, default_buffer_size));
    REQUIRE_NOTHROW(pullsp1->set_option_int(_opt_::send_buffer, default_buffer_size));
    REQUIRE_NOTHROW(pullsp2->set_option_int(_opt_::send_buffer, default_buffer_size));
    REQUIRE_NOTHROW(pullsp3->set_option_int(_opt_::send_buffer, default_buffer_size));

    unique_ptr<binary_message> abcsp, defsp;

    REQUIRE_NOTHROW(abcsp = make_unique<binary_message>());
    REQUIRE_NOTHROW(defsp = make_unique<binary_message>());

    REQUIRE_NOTHROW(*abcsp << abc);
    REQUIRE_NOTHROW(*defsp << def);

    const auto receive_timeout = 100ms;

    REQUIRE_NOTHROW(pullsp1->set_option_usec(_opt_::receive_timeout_usec, CAST_DURATION_TO_USEC(receive_timeout).count()));
    REQUIRE_NOTHROW(pullsp2->set_option_usec(_opt_::receive_timeout_usec, CAST_DURATION_TO_USEC(receive_timeout).count()));
    REQUIRE_NOTHROW(pullsp3->set_option_usec(_opt_::receive_timeout_usec, CAST_DURATION_TO_USEC(receive_timeout).count()));

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
    REQUIRE_THAT(abcsp->body()->get(), Equals(abc_buf));
    REQUIRE_NOTHROW(pullsp2->try_receive(defsp.get()));
    REQUIRE_THAT(defsp->body()->get(), Equals(def_buf));

    // Yes, re-make the binary messages.
    REQUIRE_NOTHROW(abcsp = make_unique<binary_message>());
    REQUIRE_NOTHROW(defsp = make_unique<binary_message>());

    REQUIRE_THROWS_AS_MATCHING(pullsp1->try_receive(abcsp.get()), nng_exception, ThrowsNngException(ec_etimedout));
    REQUIRE_THROWS_AS_MATCHING(pullsp2->try_receive(defsp.get()), nng_exception, ThrowsNngException(ec_etimedout));

    SECTION("Can close sockets") {
        REQUIRE_NOTHROW(pushsp.reset());
        REQUIRE_NOTHROW(pullsp1.reset());
        REQUIRE_NOTHROW(pullsp2.reset());
        REQUIRE_NOTHROW(pullsp3.reset());
    }
}
