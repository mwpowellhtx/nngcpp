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

#include "../catch/catch_tags.h"
#include "../helpers/constants.h"

#include "transport.h"

#include <core/listener.h>
#include <core/dialer.h>
#include <algorithms/string_algo.hpp>

#include <cstdlib>
#include <sstream>
#include <algorithm>

#include <iostream>

namespace nng {

    int address_calculator::port = TEST_PORT;

    address_calculator::address_calculator(char port_delim) : _port_delim() {
        if (port_delim) {
            _port_delim.push_back(port_delim);
        }
    }

    address_calculator::~address_calculator() {
    }

    uint16_t address_calculator::get_port(int delta) {
        return port += delta;
    }

    uint16_t address_calculator::get_current_port() {
        return port;
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

    // TODO: TBD: this is a likely candidate for everything besides just transport.
    void init_random_buffer(buffer_vector_type& buf, const size_type sz) {

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

    c_style_transport_fixture_redeux::c_style_transport_fixture_redeux()
        : transport_fixture_redeux_base(0, 0) {
        REQUIRE(::nng_rep_open(&_rep) == 0);
        REQUIRE(::nng_req_open(&_req) == 0);
    }

    c_style_transport_fixture_redeux::~c_style_transport_fixture_redeux() {
        if (_rep) REQUIRE(::nng_close(_rep) == 0);
        if (_req) REQUIRE(::nng_close(_req) == 0);
        _rep = _req = 0;
    }

    transport_fixture_redeux::transport_fixture_redeux()
        : transport_fixture_redeux_base() {

        using namespace protocol;
        _req.reset(new latest_req_socket());
        _rep.reset(new latest_rep_socket());
    }

    transport_fixture_redeux::~transport_fixture_redeux() {
        REQUIRE_NOTHROW(_req.reset());
        REQUIRE_NOTHROW(_rep.reset());
    }
}

namespace constants {

    const std::string ping = "ping";
    const std::string acknowledge = "acknowledge";

    const auto ping_buf = to_buffer(ping);
    const auto acknowledge_buf = to_buffer(acknowledge);
}


TEST_CASE("Test the transport using C++ wrappers", Catch::Tags(constants::prefix_tags
    , "rep", "req", "transport", "nng", "cxx", "sample").c_str()) {

    using namespace std;
    using namespace nng;
    using namespace nng::protocol;
    //using namespace nng::messaging;
    using namespace nng::exceptions;
    using namespace constants;
    using namespace Catch::Matchers;
    using O = option_names;

    auto calc = address_calculator(port_delim);

    const auto addr = calc.get_next_addr(test_addr_base);

    init(addr);

    std::unique_ptr<latest_rep_socket> repp;
    std::unique_ptr<latest_req_socket> reqp;

    REQUIRE_NOTHROW(repp = make_unique<latest_rep_socket>());
    REQUIRE_NOTHROW(reqp = make_unique<latest_req_socket>());

    SECTION("Connection refused works") {

        WARN("Connection refused works: '" + addr + "'");

        dialer d;

        REQUIRE_THROWS_AS_MATCHING(reqp->dial(addr, &d), nng_exception, THROWS_NNG_EXCEPTION(ec_econnrefused));
        REQUIRE_THROWS_AS_MATCHING(repp->dial(addr, &d), nng_exception, THROWS_NNG_EXCEPTION(ec_econnrefused));
    }

    SECTION("Duplicate listeners rejected") {

        WARN("Duplicate listeners rejected: '" + addr + "'");

        // Smart pointers is overkill for these sections.
        listener l1, l2;

        REQUIRE_NOTHROW(repp->listen(addr, &l1));
        REQUIRE(l1.has_one() == true);

        // Leaving the first Listener open and available.
        REQUIRE_THROWS_AS_MATCHING(reqp->listen(addr, &l2), nng_exception, THROWS_NNG_EXCEPTION(ec_eaddrinuse));
        REQUIRE(l2.has_one() == false);
    }

    SECTION("Listener and dialer accepted") {

        WARN("Listener and dialer accepted: '" + addr + "'");

        // Smart pointers is overkill for these sections.
        listener l;
        dialer d;

        REQUIRE_NOTHROW(repp->listen(addr, &l));
        REQUIRE(l.has_one() == true);

        REQUIRE_NOTHROW(reqp->dial(addr, &d));
        REQUIRE(d.has_one() == true);
    }

    SECTION("Send and receive") {

        WARN("Send and receive: '" + addr + "'");

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
        REQUIRE_NOTHROW(*sendp << ping);
        REQUIRE_NOTHROW(reqp->send(sendp.get()));

        REQUIRE_NOTHROW(recvp = make_unique<binary_message>(nullptr));
        REQUIRE_NOTHROW(repp->try_receive(recvp.get()));
        REQUIRE_THAT(recvp->body()->get(), Equals(ping_buf));

        REQUIRE_NOTHROW(recvp->body()->ltrim(ping.length()));
        REQUIRE_NOTHROW(*recvp << acknowledge);
        REQUIRE_NOTHROW(repp->send(recvp.get()));
        REQUIRE_NOTHROW(reqp->try_receive(sendp.get()));
        REQUIRE_THAT(sendp->body()->get(), Equals(acknowledge_buf));

        REQUIRE_NOTHROW(pp = make_unique<message_pipe>(sendp.get()));
        REQUIRE(pp->has_one() == true);

        // TODO: TBD: this bit is borderline message pipe unit testing and that's about it...
        REQUIRE_NOTHROW(actual_addr.resize(NNG_MAXADDRLEN));
        REQUIRE_NOTHROW(pp->options()->get(O::url, actual_addr));
        REQUIRE(actual_addr == addr);
    }

    SECTION("Send and receive large data") {

        WARN("Send and receive large data: '" + addr + "'");

        listener l;
        dialer d;

        unique_ptr<binary_message> sendp, recvp;

        REQUIRE_NOTHROW(seed_random_generator(time(static_cast<std::time_t*>(nullptr))));

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
        REQUIRE_NOTHROW(recvp = make_unique<binary_message>());
        REQUIRE_NOTHROW(*recvp << data);
        REQUIRE_NOTHROW(repp->send(recvp.get()));
        REQUIRE_NOTHROW(sendp = make_unique<binary_message>(nullptr));
        REQUIRE_NOTHROW(reqp->try_receive(sendp.get()));
        // Ditto excessive size truncation.
        REQUIRE_THAT(sendp->body()->get(), Equals(data));
    }
}

TEST_CASE("Test the transport in C style", Catch::Tags(constants::prefix_tags
    , "rep", "req", "transport", "nng", "c", "sample").c_str()) {

    using namespace std;
    using namespace trx;
    using namespace nng;
    using namespace nng::protocol;
    using namespace constants;
    using namespace Catch::Matchers;
    using O = option_names;

    address_calculator calc(port_delim);

    const auto addr = calc.get_next_addr(test_addr_base);

    init(addr);

    c_style_transport_fixture_redeux redeux;

    const auto& req = redeux._req;
    const auto& rep = redeux._rep;

    SECTION("Connection refused works") {

        WARN("Connection refused works: '" + addr + "'");

        ::nng_dialer d = 0;

        REQUIRE(::nng_dial(req, addr.c_str(), &d, 0) == ::NNG_ECONNREFUSED);
        REQUIRE(!d);

        REQUIRE(::nng_dial(rep, addr.c_str(), &d, 0) == ::NNG_ECONNREFUSED);
        REQUIRE(!d);
    }

    SECTION("Duplicate listen rejected") {

        WARN("Duplicate listen rejected: '" + addr + "'");

        ::nng_listener l = 0;

        REQUIRE(::nng_listen(rep, addr.c_str(), &l, 0) == 0);
        // TODO: TBD: close the listener before the next attempt? or just let it be?
        REQUIRE(l);

        l = 0;
        REQUIRE(::nng_listen(req, addr.c_str(), &l, 0) == ::NNG_EADDRINUSE);
        REQUIRE(!l);
    }

    SECTION("Listener and dialer accepted") {

        WARN("Listener and dialer accepted: '" + addr + "'");

        // DUH! Listener and a _D_I_A_L_E_R_!
        ::nng_listener l;
        ::nng_dialer d;

        REQUIRE(::nng_listen(rep, addr.c_str(), &l, 0) == 0);
        REQUIRE(l);

        REQUIRE(::nng_dial(req, addr.c_str(), &d, 0) == 0);
        REQUIRE(d);
    }

    SECTION("Send and receive") {

        WARN("Send and receive: '" + addr + "'");

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

        REQUIRE_NOTHROW(*sendp << ping);
        REQUIRE(::nng_sendmsg(req, sendp->get_msgp(), 0) == 0);
        REQUIRE_NOTHROW(sendp->on_sent());
        msgp = nullptr;
        REQUIRE(::nng_recvmsg(rep, &msgp, 0) == 0);
        REQUIRE(msgp);
        REQUIRE_NOTHROW(recvp->set_msgp(msgp));
        REQUIRE_THAT(recvp->body()->get(), Equals(ping_buf));

        REQUIRE_NOTHROW(recvp->body()->rtrim(ping.size()));
        REQUIRE_NOTHROW(*recvp << acknowledge);
        REQUIRE(::nng_sendmsg(rep, recvp->get_msgp(), 0) == 0);
        REQUIRE_NOTHROW(recvp->on_sent());
        msgp = nullptr;
        REQUIRE(::nng_recvmsg(req, &msgp, 0) == 0);
        REQUIRE(msgp);
        REQUIRE_NOTHROW(sendp->set_msgp(msgp));
        REQUIRE_THAT(sendp->body()->get(), Equals(acknowledge_buf));

        REQUIRE_NOTHROW(p = ::nng_msg_get_pipe(sendp->get_msgp()));
        REQUIRE(p);

        REQUIRE_NOTHROW(url.resize(NNG_MAXADDRLEN));
        REQUIRE(::nng_pipe_getopt(p, O::url.c_str(), &url[0], &(sz = NNG_MAXADDRLEN)) == 0);
        REQUIRE(sz < NNG_MAXADDRLEN);
        REQUIRE_NOTHROW(url.resize(sz - 1));
        REQUIRE_THAT(url, Equals(string(addr)));
    }
}
