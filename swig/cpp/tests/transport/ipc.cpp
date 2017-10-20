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

#include "transport.h"

#include "../helpers/constants.h"

#include <nngcpp.h>

#include <core/core.h>
#include <messaging/messaging.h>

#include <iostream>

namespace constants {
    // TODO: TBD: I wonder if this ultimately refers to actually /tmp/nng_ipc_test_ in the file path?
    // TODO: TBD: that would be on Linux, I'm assuming; what about on Windows? is the implementation "there" yet?
    //const std::string test_addr_base = "ipc:///tmp/nng_ipc_test_";
    const std::string test_addr_base = "ipc:///tmp/nng_ipc_test_";
}

TEST_CASE("IPC transport tests in C", "[ipc][transport][nng][c]") {

    using namespace nng;
    using namespace constants;

    // Everything is tucked away in the "fixture" in this instance.
    c_style_transport_fixture fixture(test_addr_base);

    fixture.run_all();
}

TEST_CASE("IPC transport tests using C++ wrapper", "[ipc][transport][nng][cxx]") {

    using namespace nng;
    using namespace constants;

    SECTION("Running test fixture for address base: '" + test_addr_base + "'") {

        // Everything is tucked away in the "fixture" in this instance.
        transport_fixture fixture(test_addr_base);

        fixture.run_all();
    }
}

namespace constants {

    const std::string ping2 = "ping";
    const std::string acknowledge2 = "acknowledge";

    const auto ping2_buf = to_buffer(ping2);
    const auto acknowledge2_buf = to_buffer(acknowledge2);
}

namespace nng {
    namespace messaging {

        // TODO: TBD: this is a likely candidate for everything besides just transport.
        extern void init_random_buffer(buffer_vector_type& buf, const size_type sz);

        extern void twos_compliment_buffer(buffer_vector_type& buf);
    }
}

TEST_CASE("IPC transport tests using C++ wrapper alternate approach", "[abc]") {

    using namespace std;
    using namespace nng;
    using namespace nng::protocol;
    using namespace nng::messaging;
    using namespace constants;
    using namespace Catch::Matchers;
    using O = option_names;

    transport_fixture fixture(test_addr_base);

    SECTION("Given the sockets and next address") {

        unique_ptr<latest_rep_socket> repp;
        unique_ptr<latest_req_socket> reqp;

        REQUIRE_NOTHROW(repp = make_unique<latest_rep_socket>());
        REQUIRE_NOTHROW(reqp = make_unique<latest_req_socket>());

        const auto addr = fixture.get_next_addr(test_addr_base);

        std::cout << "addr: " << addr << std::endl;

        SECTION("Connection refused works") {

            dialer d;

            REQUIRE_THROWS_AS_MATCHING(reqp->dial(addr, &d), nng_exception, THROWS_NNG_EXCEPTION(ec_econnrefused));
            REQUIRE_THROWS_AS_MATCHING(repp->dial(addr, &d), nng_exception, THROWS_NNG_EXCEPTION(ec_econnrefused));
        }

        SECTION("Duplicate listeners rejected") {

            // Smart pointers is overkill for these sections.
            listener l1, l2;

            REQUIRE_NOTHROW(repp->listen(addr, &l1));
            REQUIRE(l1.has_one() == true);

            // Leaving the first Listener open and available.
            REQUIRE_THROWS_AS_MATCHING(reqp->listen(addr, &l2), nng_exception, THROWS_NNG_EXCEPTION(ec_eaddrinuse));
            REQUIRE(l2.has_one() == false);
        }

        SECTION("Listener and dialer accepted") {

            // Smart pointers is overkill for these sections.
            listener l;
            dialer d;

            REQUIRE_NOTHROW(repp->listen(addr, &l));
            REQUIRE(l.has_one() == true);

            REQUIRE_NOTHROW(reqp->dial(addr, &d));
            REQUIRE(d.has_one() == true);
        }

        SECTION("Send and receive") {

            FAIL();

            listener l;
            dialer d;

            unique_ptr<message_pipe> pp;
            unique_ptr<binary_message> sendp, recvp;

            string actual_addr;

            REQUIRE_NOTHROW(repp->listen(addr, &l));
            REQUIRE(l.has_one() == true);
            REQUIRE_NOTHROW(reqp->dial(addr, &d));
            REQUIRE(d.has_one() == true);

            // Sleep so listener catches up, may be running slightly behind.
            SLEEP_FOR(20ms);

            REQUIRE_NOTHROW(sendp = make_unique<binary_message>());
            REQUIRE_NOTHROW(*sendp << ping2);
            REQUIRE_NOTHROW(reqp->send(sendp.get()));

            REQUIRE_NOTHROW(recvp = make_unique<binary_message>(nullptr));
            REQUIRE_NOTHROW(repp->try_receive(recvp.get()));
            REQUIRE_THAT(recvp->body()->get(), Equals(ping2_buf));

            REQUIRE_NOTHROW(*sendp << acknowledge2);
            REQUIRE_NOTHROW(repp->send(sendp.get()));
            REQUIRE_NOTHROW(recvp->set_msgp(nullptr));
            REQUIRE_NOTHROW(reqp->try_receive(recvp.get()));
            REQUIRE_THAT(recvp->body()->get(), Equals(acknowledge2_buf));

            REQUIRE_NOTHROW(pp = make_unique<message_pipe>(recvp.get()));
            REQUIRE(pp->has_one() == true);

            // TODO: TBD: this bit is borderline message pipe unit testing and that's about it...
            REQUIRE_NOTHROW(actual_addr.resize(addr.length()));
            REQUIRE_NOTHROW(pp->get_option(O::url, actual_addr));
            REQUIRE(actual_addr == addr);
        }

        SECTION("Send and receive large data") {

            INFO("For address: " + addr);
            FAIL();

            listener l;
            dialer d;

            unique_ptr<binary_message> sendp, recvp;

            // What's a buffer without some HEX involvement...
            const size_type sz = 0x400 * 0x80; // Much larger than any transport segment.
            buffer_vector_type data;

            REQUIRE_NOTHROW(init_random_buffer(data, sz));

            REQUIRE_NOTHROW(repp->listen(addr, &l));
            REQUIRE(l.has_one() == true);

            REQUIRE_NOTHROW(reqp->dial(addr, &d));
            REQUIRE(d.has_one() == true);

            // Wait for listener to catch up since it may be slightly behind.
            SLEEP_FOR(20ms);

            REQUIRE_NOTHROW(sendp = make_unique<binary_message>());
            REQUIRE_NOTHROW(*sendp << data);
            REQUIRE_NOTHROW(reqp->send(sendp.get()));
            REQUIRE_NOTHROW(recvp = make_unique<binary_message>(nullptr));
            REQUIRE_NOTHROW(repp->try_receive(recvp.get()));
            // Heaven help us if this fails: expect report to be truncated due to excessive size.
            REQUIRE_THAT(recvp->body()->get(), Equals(data));

            REQUIRE_NOTHROW(twos_compliment_buffer(data));
            REQUIRE_NOTHROW(sendp = make_unique<binary_message>());
            REQUIRE_NOTHROW(*sendp << data);
            REQUIRE_NOTHROW(repp->send(sendp.get()));
            REQUIRE_NOTHROW(recvp = make_unique<binary_message>(nullptr));
            REQUIRE_NOTHROW(reqp->try_receive(recvp.get()));
            // Ditto excessive size truncation.
            REQUIRE_THAT(recvp->body()->get(), Equals(data));
        }
    }
}

TEST_CASE("IPC transport tests using C style alternate approach", "[def]") {

    using namespace std;
    using namespace nng;
    using namespace nng::protocol;
    using namespace nng::messaging;
    using namespace constants;
    using namespace Catch::Matchers;
    using O = option_names;

    c_style_transport_fixture fixture(test_addr_base);

    ::nng_socket& req = fixture._req;
    ::nng_socket& rep = fixture._rep;

    const auto addr = fixture.get_next_addr(test_addr_base);

    std::cout << "addr: " << addr << std::endl;

    SECTION("Connection refused works") {
        ::nng_dialer d = 0;
        REQUIRE(::nng_dial(req, addr.c_str(), &d, 0) == ::NNG_ECONNREFUSED);
        REQUIRE(!d);
        REQUIRE(::nng_dial(rep, addr.c_str(), &d, 0) == ::NNG_ECONNREFUSED);
        REQUIRE(!d);
    }

    SECTION("Duplicate listen rejected") {

        ::nng_listener l;

        REQUIRE(::nng_listen(rep, addr.c_str(), &l, 0) == 0);
        // TODO: TBD: close the listener before the next attempt? or just let it be?
        REQUIRE(l);

        l = 0;
        REQUIRE(::nng_listen(req, addr.c_str(), &l, 0) == ::NNG_EADDRINUSE);
        REQUIRE(!l);
    }

    SECTION("Listener and dialer accepted") {

        // DUH! Listener and a _D_I_A_L_E_R_!
        ::nng_listener l;
        ::nng_dialer d;

        REQUIRE(::nng_listen(rep, addr.c_str(), &l, 0) == 0);
        REQUIRE(l);

        REQUIRE(::nng_dial(req, addr.c_str(), &d, 0) == 0);
        REQUIRE(d);
    }

    SECTION("Send and receive") {

        FAIL();

        ::nng_listener l = 0;
        ::nng_dialer d = 0;
        ::nng_pipe p = 0;

        string url;
        string::size_type sz;

        /* This is a hybrid test. The primary purpose of this test is to exercise the core socket oriented
        methodology apart from the C++ wrappers. Engaging with the C API, however, I am confident of the
        messaging work. Just not so much of the transport at the moment. Just be careful of the message
        ownership semantics and it ought to be fine. */

        ::nng_msg* msgp;
        unique_ptr<binary_message> sendp, recvp;

        REQUIRE_NOTHROW(sendp = make_unique<binary_message>());
        REQUIRE_NOTHROW(recvp = make_unique<binary_message>(nullptr));

        /* TODO: TBD: definitely IPC is hanging up on this step during the _D_I_A_L_
        on account of what looks to be an internal transport API wait on a Condition
        Variable. The only different from what I can determine is that the unit tests
        engage with an internal nni_tran_find call, which is not exposed to us for
        public consumption. Submitted issue to the repo:
        http://github.com/nanomsg/nng/issues/117 */

        REQUIRE(::nng_listen(rep, addr.c_str(), &l, 0) == 0);
        REQUIRE(l);
        REQUIRE(::nng_dial(req, addr.c_str(), &d, 0) == 0);
        REQUIRE(d);

        // Ditto allowing listener to catch up.
        SLEEP_FOR(20ms);

        REQUIRE_NOTHROW(*sendp << ping2);
        REQUIRE(::nng_sendmsg(req, sendp->get_msgp(), 0) == 0);
        REQUIRE(::nng_recvmsg(rep, &(msgp = nullptr), 0) == 0);
        REQUIRE(msgp);
        REQUIRE_NOTHROW(recvp->set_msgp(msgp));
        REQUIRE_THAT(recvp->body()->get(), Equals(ping2_buf));

        REQUIRE_NOTHROW(sendp->set_msgp(nullptr));
        REQUIRE_NOTHROW(*sendp << acknowledge2);
        REQUIRE(::nng_sendmsg(rep, sendp->get_msgp(), 0) == 0);
        REQUIRE(::nng_recvmsg(req, &(msgp = nullptr), 0) == 0);
        REQUIRE(msgp);
        REQUIRE_NOTHROW(recvp->set_msgp(msgp));
        REQUIRE_THAT(recvp->body()->get(), Equals(acknowledge2_buf));

        REQUIRE_NOTHROW(p = ::nng_msg_get_pipe(recvp->get_msgp()));
        REQUIRE(p);

        url.resize(NNG_MAXADDRLEN);
        REQUIRE(::nng_pipe_getopt(p, O::url.c_str(), &url[0], &(sz = NNG_MAXADDRLEN)) == 0);
        REQUIRE_THAT(url, Equals(string(addr)));
    }
}
