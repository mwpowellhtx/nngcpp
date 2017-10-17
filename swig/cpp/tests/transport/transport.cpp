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
#include "../helpers/constants.h"

#include "transport.h"

#include <cstdlib>
#include <sstream>
#include <algorithm>

namespace constants {

    const std::string ping = "ping";
    const std::string acknowledge = "acknowledge";

    const auto ping_buf = to_buffer(ping);
    const auto acknowledge_buf = to_buffer(acknowledge);
}

namespace nng {

    // TODO: TBD: could expose this, but random generators are a dime a dozen...
    void seed_random_generator(std::time_t seed) {
        // Initialize the random generator.
        std::srand(static_cast<int>(seed));
    }

    // TODO: TBD: we could potentially accept command line arguments for the PORT, but I really fail to see the need
    transport_fixture::transport_fixture(const std::string& base_addr)
        : _port(TEST_PORT)
        , _base_addr(base_addr)
        , _reqp()
        , _repp() {

        REQUIRE_NOTHROW(seed_random_generator(std::time(static_cast<std::time_t*>(nullptr))));

        REQUIRE_NOTHROW(_reqp = std::make_unique<protocol::latest_req_socket>());
        REQUIRE_NOTHROW(_repp = std::make_unique<protocol::latest_rep_socket>());
    }

    transport_fixture::~transport_fixture() {
    }

    int transport_fixture::get_port(int delta) {
        auto port = _port;
        _port += delta;
        return port;
    }

    std::string transport_fixture::get_addr(int delta) {
        std::ostringstream os;
        os << _base_addr << ":" << get_port(delta);
        return os.str();
    }

    std::string transport_fixture::get_next_addr(int delta) {
        return get_addr(std::abs(delta));
    }

    std::string transport_fixture::get_prev_addr(int delta) {
        return get_addr(-std::abs(delta));
    }

    void transport_fixture::run_all() {

        const auto addr = get_next_addr();

        //("Given transport for address " + _current_addr).c_str()
        SECTION("Given transport for address '" + addr + "'") {
            run_connection_refused_works(addr);
            run_dup_listeners_rejected(addr);
            run_listener_and_dialer_accepted(addr);
            run_send_and_receive(addr);
            run_send_and_receive_large_data(addr);
        }
    }

    void transport_fixture::run_connection_refused_works(const std::string& addr) {

        using namespace std;
        using namespace protocol;
        using namespace Catch::Matchers;

        SECTION("Connection refused works") {

            unique_ptr<dialer> dp;

            REQUIRE_NOTHROW(dp = make_unique<dialer>());
            REQUIRE_THROWS_AS_MATCHING(_reqp->dial(addr, dp.get()), nng_exception, ThrowsNngException(ec_econnrefused));

            REQUIRE_NOTHROW(dp = make_unique<dialer>());
            REQUIRE_THROWS_AS_MATCHING(_repp->dial(addr, dp.get()), nng_exception, ThrowsNngException(ec_econnrefused));
        }
    }

    void transport_fixture::run_dup_listeners_rejected(const std::string& addr) {

        using namespace std;
        using namespace protocol;
        using namespace Catch::Matchers;

        SECTION("Duplicate listeners rejected") {

            unique_ptr<listener> lp1, lp2;

            REQUIRE_NOTHROW(lp1 = make_unique<listener>());
            REQUIRE_NOTHROW(_repp->listen(addr, lp1.get()));
            REQUIRE(lp1->has_one() == true);

            // Leaving the first Listener open and available.
            REQUIRE_NOTHROW(lp2 = make_unique<listener>());
            REQUIRE_THROWS_AS_MATCHING(_reqp->listen(addr, lp2.get()), nng_exception, ThrowsNngException(ec_eaddrinuse));
            REQUIRE(lp2->has_one() == false);
        }
    }

    void transport_fixture::run_listener_and_dialer_accepted(const std::string& addr) {

        using namespace std;
        using namespace protocol;

        SECTION("Listener and dialer accepted") {

            unique_ptr<listener> lp;
            unique_ptr<dialer> dp;

            REQUIRE_NOTHROW(lp = make_unique<listener>());
            REQUIRE_NOTHROW(_repp->listen(addr, lp.get()));
            REQUIRE(lp->has_one() == true);

            REQUIRE_NOTHROW(dp = make_unique<dialer>());
            REQUIRE_NOTHROW(_reqp->dial(addr, dp.get()));
            REQUIRE(dp->has_one() == true);
        }
    }

    void transport_fixture::run_send_and_receive(const std::string& addr) {

        using namespace std;
        using namespace protocol;
        using namespace messaging;
        using namespace constants;
        using namespace Catch::Matchers;
        using O = option_names;
 
        SECTION("Send and receive") {

            unique_ptr<listener> lp;
            unique_ptr<dialer> dp;

            unique_ptr<binary_message> sendp, recvp;
            unique_ptr<message_pipe> pp;

            std::string actual_addr;

            REQUIRE_NOTHROW(lp = make_unique<listener>());
            REQUIRE_NOTHROW(dp = make_unique<dialer>());

            REQUIRE_NOTHROW(_repp->listen(addr, lp.get()));
            REQUIRE(lp->has_one() == true);
            REQUIRE_NOTHROW(_reqp->dial(addr, dp.get()));
            REQUIRE(dp->has_one() == true);

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
        void init_random_buffer(message_base::buffer_vector_type& buffer
            ,  const message_base::size_type sz) {

            buffer.resize(sz);

            for (message_base::size_type i = 0; i < sz; i++) {
                buffer[i] = std::rand() & 0xff;
            }
        }

        void twos_compliment_buffer(message_base::buffer_vector_type& buffer) {
            const auto &twos_compliment = [](message_base::buffer_vector_type::value_type& x) {
                x = ~x;
            };
            std::for_each(buffer.begin(), buffer.end(), twos_compliment);
        }
    }

    void transport_fixture::run_send_and_receive_large_data(const std::string& addr) {

        using namespace std;
        using namespace messaging;
        using namespace Catch::Matchers;

        SECTION("Send and receive large data") {

            unique_ptr<listener> lp;
            unique_ptr<dialer> dp;

            unique_ptr<binary_message> sendp, recvp;

            // What's a buffer without some HEX involvement...
            const message_base::size_type sz = 0x400 * 0x80; // Much larger than any transport segment.
            message_base::buffer_vector_type data;

            REQUIRE_NOTHROW(init_random_buffer(data, sz));

            REQUIRE_NOTHROW(lp = make_unique<listener>());
            REQUIRE_NOTHROW(_repp->listen(addr, lp.get()));
            REQUIRE(lp->has_one() == true);

            REQUIRE_NOTHROW(dp = make_unique<dialer>());
            REQUIRE_NOTHROW(_reqp->dial(addr, dp.get()));
            REQUIRE(dp->has_one() == true);

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
}
