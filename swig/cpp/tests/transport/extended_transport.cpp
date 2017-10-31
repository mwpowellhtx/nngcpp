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

#include "extended_transport.h"

#include "core/core.h"

#include "../helpers/constants.h"

namespace constants {
    const std::string props = "props";
    const nng::buffer_vector_type props_buf = to_buffer(props);
}

TEST_CASE("Check some properties", "[check][properties][nng][cxx][bonus]") {

    using namespace std;
    using namespace nng;
    using namespace nng::protocol;
    using namespace constants;
    using namespace Catch::Matchers;
    using O = option_names;

    address_calculator calc(port_delim);
    const auto get_current_port = std::bind(&address_calculator::get_current_port);

    unique_ptr<binary_message> sendp, recvp;
    transport_fixture_redeux redeux;

    const auto addr = calc.get_next_addr(test_addr_base);

    const auto& rep = redeux._rep;
    const auto& req = redeux._req;

    SECTION("Properties tests") {

        WARN("Running properties test on address: '" + addr + "'");

        listener l;
        dialer d;

        unique_ptr<binary_message> sendp, recvp;

        REQUIRE_NOTHROW(sendp = make_unique<binary_message>());
        REQUIRE_NOTHROW(recvp = make_unique<binary_message>(nullptr));

        REQUIRE_NOTHROW(rep->listen(addr, &l));
        REQUIRE(l.HasOne() == true);
        REQUIRE_NOTHROW(req->dial(addr, &d));
        REQUIRE(d.HasOne() == true);
        SLEEP_FOR(20ms); // Allow listener to catch up from being slightly behind.

        REQUIRE_NOTHROW(*sendp << props);
        REQUIRE_NOTHROW(req->Send(sendp.get()));
        REQUIRE(sendp->HasOne() == false);

        REQUIRE_NOTHROW(rep->TryReceive(recvp.get()));
        REQUIRE(recvp->HasOne() == true);
        REQUIRE_THAT(recvp->GetBody()->Get(), Equals(props_buf));

        SECTION("Now test the properties") {

            unique_ptr<message_pipe> pp;

            // Vet the Pipe and the Port ahead of time.
            REQUIRE_NOTHROW(pp = make_unique<message_pipe>(recvp.get()));
            REQUIRE(pp->HasOne() == true);

            auto current_port = get_current_port();
            REQUIRE(current_port != 0);

            SECTION("Local address property works") {
                test_local_addr_properties(pp.get(), &l, &d, current_port);
            }

            SECTION("Remote address property works") {
                test_remote_addr_properties(pp.get(), &l, &d, current_port);
            }
        }
    }
}

TEST_CASE("Check some properties in C style", "[check][properties][nng][c][bonus]") {

    using namespace std;
    using namespace nng;
    using namespace constants;
    using namespace Catch::Matchers;

    address_calculator calc(port_delim);
    const auto get_current_port = std::bind(&address_calculator::get_current_port);

    unique_ptr<binary_message> sendp, recvp;
    c_style_transport_fixture_redeux redeux;

    const auto addr = calc.get_next_addr(test_addr_base);

    const auto& rep = redeux._rep;
    const auto& req = redeux._req;

    SECTION("Properties tests") {

        WARN("Running properties test on address: '" + addr + "'");

        ::nng_listener l;
        ::nng_dialer d;

        ::nng_msg* msgp;
        unique_ptr<binary_message> sendp, recvp;

        REQUIRE_NOTHROW(sendp = make_unique<binary_message>());
        REQUIRE_NOTHROW(recvp = make_unique<binary_message>(nullptr));

        REQUIRE(::nng_listen(rep, addr.c_str(), &l, 0) == 0);
        REQUIRE(l);
        REQUIRE(::nng_dial(req, addr.c_str(), &d, 0) == 0);
        REQUIRE(d);
        SLEEP_FOR(20ms); // Allow listener to catch up from being slightly behind.

        REQUIRE_NOTHROW(*sendp << props);
        REQUIRE(::nng_sendmsg(req, sendp->cede_message(), 0) == 0);
        REQUIRE(sendp->HasOne() == false);

        REQUIRE(::nng_recvmsg(rep, &msgp, 0) == 0);
        REQUIRE_NOTHROW(recvp->retain(msgp));
        REQUIRE(recvp->HasOne() == true);
        REQUIRE_THAT(recvp->GetBody()->Get(), Equals(props_buf));

        SECTION("Now test the properties") {

            // Vet the Pipe and the Port ahead of time.
            ::nng_pipe p = ::nng_msg_get_pipe(recvp->get_message());
            REQUIRE(p);

            auto current_port = get_current_port();
            REQUIRE(current_port != 0);

            SECTION("Local address property works") {
                test_local_addr_properties(&p, &l, &d, current_port);
            }

            SECTION("Remote address property works") {
                test_remote_addr_properties(&p, &l, &d, current_port);
            }
        }
    }
}
