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

#include <core/listener.h>
#include <core/dialer.h>

#include "../helpers/constants.h"

#include <cstdlib>
#include <sstream>
#include <algorithm>

#include <iostream>

namespace constants {

    const std::string ping = "ping";
    const std::string acknowledge = "acknowledge";

    const auto ping_buf = to_buffer(ping);
    const auto acknowledge_buf = to_buffer(acknowledge);
}

namespace nng {

    int address_calculator::port = TEST_PORT;

    address_calculator::address_calculator(char port_delim) : _port_delim() {
        if (port_delim) {
            _port_delim.push_back(port_delim);
        }
    }

    address_calculator::~address_calculator() {
    }

    int address_calculator::get_port(int delta) {
        return port += delta;
    }

    std::string address_calculator::get_addr(const std::string& base_addr, int delta) {
        std::ostringstream os;
        os << base_addr << _port_delim << get_port(delta);
        return os.str();
    }

    std::string address_calculator::get_next_addr(const std::string& base_addr, int delta) {
        return get_addr(base_addr, std::abs(delta));
    }

    std::string address_calculator::get_prev_addr(const std::string& base_addr, int delta) {
        return get_addr(base_addr, -std::abs(delta));
    }

    // TODO: TBD: could expose this, but random generators are a dime a dozen...
    void seed_random_generator(std::time_t seed) {
        // Initialize the random generator.
        std::srand(static_cast<int>(seed));
    }

    // TODO: TBD: we could potentially accept command line arguments for the PORT, but I really fail to see the need
    transport_fixture::transport_fixture(const std::string& base_addr, char port_delim)
        : address_calculator(port_delim)
        , _base_addr(base_addr)
        , _reqp()
        , _repp() {

        using namespace std;
        using namespace protocol;

        REQUIRE_NOTHROW(seed_random_generator(time(static_cast<std::time_t*>(nullptr))));

        REQUIRE_NOTHROW(_repp = make_unique<latest_rep_socket>());
        REQUIRE_NOTHROW(_reqp = make_unique<latest_req_socket>());
    }

    transport_fixture::~transport_fixture() {
        REQUIRE_NOTHROW(_reqp.reset());
        REQUIRE_NOTHROW(_repp.reset());
    }

    void transport_fixture::run_all() {

        const auto addr = get_next_addr(_base_addr);

        std::cout << "Given transport for address: addr = '" << addr << "'" << std::endl;

        //("Given transport for address " + _current_addr).c_str()
        SECTION("Given transport for address '" + addr + "'") {
            run_all(addr);
        }
    }

    void transport_fixture::run_one(const run_one_func& func) {
        address_calculator& self = *this;
        func(self);
    }

    void transport_fixture::run_all(const std::string& addr) {

        run_connection_refused_works(addr);
        run_dup_listeners_rejected(addr);
        run_listener_and_dialer_accepted(addr);
        run_send_and_receive(addr);
        run_send_and_receive_large_data(addr);
    }

    void transport_fixture::run_connection_refused_works(const std::string& addr) {

        SECTION("Connection refused works") {

            INFO("addr: " + addr);
            FAIL();

            dialer d;

            REQUIRE_THROWS_AS_MATCHING(_reqp->dial(addr, &d), nng_exception, THROWS_NNG_EXCEPTION(ec_econnrefused));
            REQUIRE_THROWS_AS_MATCHING(_repp->dial(addr, &d), nng_exception, THROWS_NNG_EXCEPTION(ec_econnrefused));
        }
    }

    void transport_fixture::run_dup_listeners_rejected(const std::string& addr) {

        SECTION("Duplicate listeners rejected") {

            INFO("addr: " + addr);
            FAIL();

            // Smart pointers is overkill for these sections.
            listener l1, l2;

            REQUIRE_NOTHROW(_repp->listen(addr, &l1));
            REQUIRE(l1.has_one() == true);

            // Leaving the first Listener open and available.
            REQUIRE_THROWS_AS_MATCHING(_reqp->listen(addr, &l2), nng_exception, THROWS_NNG_EXCEPTION(ec_eaddrinuse));
            REQUIRE(l2.has_one() == false);
        }
    }

    void transport_fixture::run_listener_and_dialer_accepted(const std::string& addr) {

        SECTION("Listener and dialer accepted") {

            INFO("addr: " + addr);
            FAIL();

            // Smart pointers is overkill for these sections.
            listener l;
            dialer d;

            REQUIRE_NOTHROW(_repp->listen(addr, &l));
            REQUIRE(l.has_one() == true);

            REQUIRE_NOTHROW(_reqp->dial(addr, &d, flag_none));
            //REQUIRE_NOTHROW(_reqp->dial(addr, &d));
            REQUIRE(d.has_one() == true);
        }
    }

    void transport_fixture::run_send_and_receive(const std::string& addr) {

        using namespace std;
        using namespace messaging;
        using namespace constants;
        using namespace Catch::Matchers;
        using O = option_names;
 
        SECTION("Send and receive") {

            INFO("addr: " + addr);
            FAIL();

            listener l;
            dialer d;

            unique_ptr<message_pipe> pp;
            unique_ptr<binary_message> sendp, recvp;

            string actual_addr;

            REQUIRE_NOTHROW(_repp->listen(addr, &l));
            REQUIRE(l.has_one() == true);
            REQUIRE_NOTHROW(_reqp->dial(addr, &d));
            REQUIRE(d.has_one() == true);

            // Sleep so listener catches up, may be running slightly behind.
            SLEEP_FOR(20ms);

            REQUIRE_NOTHROW(sendp = make_unique<binary_message>());
            REQUIRE_NOTHROW(*sendp << ping);
            REQUIRE_NOTHROW(_reqp->send(sendp.get()));

            REQUIRE_NOTHROW(recvp = make_unique<binary_message>(nullptr));
            REQUIRE_NOTHROW(_repp->try_receive(recvp.get()));
            REQUIRE_THAT(recvp->body()->get(), Equals(ping_buf));

            REQUIRE_NOTHROW(*sendp << acknowledge);
            REQUIRE_NOTHROW(_repp->send(sendp.get()));
            REQUIRE_NOTHROW(recvp->set_msgp(nullptr));
            REQUIRE_NOTHROW(_reqp->try_receive(recvp.get()));
            REQUIRE_THAT(recvp->body()->get(), Equals(acknowledge_buf));

            REQUIRE_NOTHROW(pp = make_unique<message_pipe>(recvp.get()));
            REQUIRE(pp->has_one() == true);

            // TODO: TBD: this bit is borderline message pipe unit testing and that's about it...
            REQUIRE_NOTHROW(actual_addr.resize(addr.length()));
            REQUIRE_NOTHROW(pp->get_option(O::url, actual_addr));
            REQUIRE(actual_addr == addr);
        }
    }

    namespace messaging {

        // TODO: TBD: this is a likely candidate for everything besides just transport.
        void init_random_buffer(buffer_vector_type& buf,  const size_type sz) {

            buf.resize(sz);

            for (size_type i = 0; i < sz; i++) {
                buf[i] = std::rand() & 0xff;
            }
        }

        void twos_compliment_buffer(buffer_vector_type& buf) {
            const auto &twos_compliment = [](buffer_vector_type::value_type& x) {
                x = ~x;
            };
            std::for_each(buf.begin(), buf.end(), twos_compliment);
        }
    }

    void transport_fixture::run_send_and_receive_large_data(const std::string& addr) {

        using namespace std;
        using namespace messaging;
        using namespace Catch::Matchers;

        SECTION("Send and receive large data") {

            listener l;
            dialer d;

            unique_ptr<binary_message> sendp, recvp;

            // What's a buffer without some HEX involvement...
            const size_type sz = 0x400 * 0x80; // Much larger than any transport segment.
            buffer_vector_type data;

            REQUIRE_NOTHROW(init_random_buffer(data, sz));

            REQUIRE_NOTHROW(_repp->listen(addr, &l));
            REQUIRE(l.has_one() == true);

            REQUIRE_NOTHROW(_reqp->dial(addr, &d));
            REQUIRE(d.has_one() == true);

            // Wait for listener to catch up since it may be slightly behind.
            SLEEP_FOR(20ms);

            REQUIRE_NOTHROW(sendp = make_unique<binary_message>());
            REQUIRE_NOTHROW(*sendp << data);
            REQUIRE_NOTHROW(_reqp->send(sendp.get()));
            REQUIRE_NOTHROW(recvp = make_unique<binary_message>(nullptr));
            REQUIRE_NOTHROW(_repp->try_receive(recvp.get()));
            // Heaven help us if this fails: expect report to be truncated due to excessive size.
            REQUIRE_THAT(recvp->body()->get(), Equals(data));

            REQUIRE_NOTHROW(twos_compliment_buffer(data));
            REQUIRE_NOTHROW(sendp = make_unique<binary_message>());
            REQUIRE_NOTHROW(*sendp << data);
            REQUIRE_NOTHROW(_repp->send(sendp.get()));
            REQUIRE_NOTHROW(recvp = make_unique<binary_message>(nullptr));
            REQUIRE_NOTHROW(_reqp->try_receive(recvp.get()));
            // Ditto excessive size truncation.
            REQUIRE_THAT(recvp->body()->get(), Equals(data));
        }
    }

    c_style_transport_fixture::c_style_transport_fixture(const std::string& base_addr, char port_delim)
        : basic_fixture()
        , address_calculator(port_delim)
        , _base_addr(base_addr)
        , _rep(0)
        , _req(0) {

        REQUIRE(::nng_pair_open(&_rep) == 0);
        REQUIRE(::nng_pair_open(&_req) == 0);
    }

    c_style_transport_fixture::~c_style_transport_fixture() {

        REQUIRE(::nng_close(_req) == 0);
        REQUIRE(::nng_close(_rep) == 0);
    }


    void c_style_transport_fixture::run_all() {

        const auto addr = get_next_addr(_base_addr);

        //("Given transport for address " + _current_addr).c_str()
        SECTION("Given transport for address '" + addr + "'") {
            run_all(addr);
        }
    }

    void c_style_transport_fixture::run_all(const std::string& addr) {

        const auto addr_c_str = addr.c_str();

        run_connection_refused_works(addr_c_str);
        run_dup_listeners_rejected(addr_c_str);
        run_listener_and_dialer_accepted(addr_c_str);
        run_send_and_receive(addr_c_str);
        run_send_and_receive_large_data(addr_c_str);
    }

    void c_style_transport_fixture::run_connection_refused_works(const char* addr) {

        SECTION("Connection refused works") {

            ::nng_dialer d = 0;

            REQUIRE(::nng_dial(_req, addr, &d, 0) == ::NNG_ECONNREFUSED);
            REQUIRE(!d);
            REQUIRE(::nng_dial(_rep, addr, &d, 0) == ::NNG_ECONNREFUSED);
            REQUIRE(!d);
        }
    }

    void c_style_transport_fixture::run_dup_listeners_rejected(const char* addr) {

        SECTION("Duplicate listen rejected") {

            ::nng_listener l;

            REQUIRE(::nng_listen(_rep, addr, &l, 0) == 0);
            // TODO: TBD: close the listener before the next attempt? or just let it be?
            REQUIRE(l);

            l = 0;
            REQUIRE(::nng_listen(_req, addr, &l, 0) == ::NNG_EADDRINUSE);
            REQUIRE(!l);
        }
    }
        
    void c_style_transport_fixture::run_listener_and_dialer_accepted(const char* addr) {

        SECTION("Listener and dialer accepted") {

            // DUH! Listener and a _D_I_A_L_E_R_!
            ::nng_listener l;
            ::nng_dialer d;

            REQUIRE(::nng_listen(_rep, addr, &l, 0) == 0);
            REQUIRE(l);

            REQUIRE(::nng_dial(_req, addr, &d, 0) == 0);
            REQUIRE(d);
        }
    }

    void c_style_transport_fixture::run_send_and_receive(const char* addr) {

        using namespace std;
        using namespace messaging;
        using namespace constants;
        using namespace Catch::Matchers;
        using O = option_names;

        SECTION("Send and receive") {

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

            REQUIRE(::nng_listen(_rep, addr, &l, 0) == 0);
            REQUIRE(l);
            REQUIRE(::nng_dial(_req, addr, &d, 0) == 0);
            REQUIRE(d);

            // Ditto allowing listener to catch up.
            SLEEP_FOR(20ms);

            REQUIRE_NOTHROW(*sendp << ping);
            REQUIRE(::nng_sendmsg(_req, sendp->get_msgp(), 0) == 0);
            REQUIRE(::nng_recvmsg(_rep, &(msgp = nullptr), 0) == 0);
            REQUIRE(msgp);
            REQUIRE_NOTHROW(recvp->set_msgp(msgp));
            REQUIRE_THAT(recvp->body()->get(), Equals(ping_buf));

            REQUIRE_NOTHROW(sendp->set_msgp(nullptr));
            REQUIRE_NOTHROW(*sendp << acknowledge);
            REQUIRE(::nng_sendmsg(_rep, sendp->get_msgp(), 0) == 0);
            REQUIRE(::nng_recvmsg(_req, &(msgp = nullptr), 0) == 0);
            REQUIRE(msgp);
            REQUIRE_NOTHROW(recvp->set_msgp(msgp));
            REQUIRE_THAT(recvp->body()->get(), Equals(acknowledge_buf));

            REQUIRE_NOTHROW(p = ::nng_msg_get_pipe(recvp->get_msgp()));
            REQUIRE(p);

            url.resize(NNG_MAXADDRLEN);
            REQUIRE(::nng_pipe_getopt(p, O::url.c_str(), &url[0], &(sz = NNG_MAXADDRLEN)) == 0);
            REQUIRE_THAT(url, Equals(string(addr)));
        }
    }

    void c_style_transport_fixture::run_send_and_receive_large_data(const char* addr) {}
}
