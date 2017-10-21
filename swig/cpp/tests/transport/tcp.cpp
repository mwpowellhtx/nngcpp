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

#include "../catch/catch_exception_matcher_base.hpp"
#include "../catch/catch_exception_translations.hpp"
#include "../helpers/constants.h"

#define NNG_ONLY
#include <nngcpp.h>

#include <core/core.h>

#ifdef _WIN32
#   include <WinSock2.h>
#else
#   include <arpa/inet.h>
#endif

#include <functional>

void init(const std::string& addr) {
}

namespace constants {

    const std::string loopback_addr_base = "tcp://127.0.0.1";
    const std::string wildcard_addr_base = "tcp://*";

    const std::string test_addr_base = loopback_addr_base;
    const char port_delim = ':';
}

namespace nng {
    namespace messaging {

        using O = option_names;

        template<>
        void test_local_addr_properties<::nng_pipe, ::nng_listener, ::nng_dialer>(::nng_pipe* const pp
            , ::nng_listener* const lp, ::nng_dialer* const dp, uint16_t expected_port) {

            ::nng_sockaddr a;
            size_type sz = sizeof(a);

            REQUIRE(::nng_pipe_getopt(*pp, O::local_address.c_str(), (void*)&a, &sz) == 0);
            REQUIRE(sz == sizeof(a));
            REQUIRE(a.s_un.s_family == ::NNG_AF_INET);
            REQUIRE(a.s_un.s_in.sa_addr == ::htonl(INADDR_LOOPBACK));
            REQUIRE(a.s_un.s_in.sa_port == ::htons(expected_port));
        }

        template<>
        void test_remote_addr_properties<::nng_pipe, ::nng_listener, ::nng_dialer>(::nng_pipe* const pp
            , ::nng_listener* const lp, ::nng_dialer* const dp, uint16_t expected_port) {

            ::nng_sockaddr a;
            size_type sz = sizeof(a);

            REQUIRE(::nng_pipe_getopt(*pp, O::remote_address.c_str(), (void*)&a, &sz) == 0);
            REQUIRE(sz == sizeof(a));
            REQUIRE(a.s_un.s_family == ::NNG_AF_INET);
            REQUIRE(a.s_un.s_in.sa_addr == ::htonl(INADDR_LOOPBACK));
            REQUIRE(a.s_un.s_in.sa_port != 0);
        }

        template<>
        void test_local_addr_properties<message_pipe, listener, dialer>(message_pipe* const pp
            , listener* const lp, dialer* const dp, uint16_t expected_port) {

            // TODO: TBD: call address socket_address instead... would be more specific.
            address a;

            REQUIRE_NOTHROW(pp->get_option(O::local_address, &a));
            REQUIRE(a.get_family() == af_inet);
            auto vp = a.view();
            REQUIRE(vp->get_family() == a.get_family());
            REQUIRE(vp->get_addr() == INADDR_LOOPBACK);
            REQUIRE(vp->get_port() == expected_port);
        }

        template<>
        void test_remote_addr_properties<message_pipe, listener, dialer>(message_pipe* const pp
            , listener* const lp, dialer* const dp, uint16_t expected_port) {

            // TODO: TBD: call address socket_address instead... would be more specific.
            address a;

            REQUIRE_NOTHROW(pp->get_option(O::remote_address, &a));
            REQUIRE(a.get_family() == af_inet);
            auto vp = a.view();
            REQUIRE(vp->get_family() == a.get_family());
            REQUIRE(vp->get_addr() == INADDR_LOOPBACK);
            REQUIRE(vp->get_port() != 0);
        }
    }
}

TEST_CASE("We cannot connect to wildcards", "[tcp][pair][connect][wildcards][transport][nng][cxx]") {

    using namespace std;
    using namespace nng;
    using namespace nng::protocol;
    using namespace constants;
    using namespace Catch::Matchers;

    std::string addr;
    address_calculator calc(port_delim);
    unique_ptr<latest_pair_socket> sp;

    REQUIRE_NOTHROW(sp = make_unique<latest_pair_socket>());

    REQUIRE_NOTHROW(addr = calc.get_next_addr(wildcard_addr_base));

    WARN("Dialing address: '" + addr + "'");
    REQUIRE_THROWS_AS_MATCHING(sp->dial(addr), nng_exception, THROWS_NNG_EXCEPTION(ec_eaddrinval));
}

TEST_CASE("We can bind to wildcards", "[tcp][pair][connect][wildcards][transport][nng][cxx]") {

    using namespace std;
    using namespace nng;
    using namespace nng::protocol;
    using namespace constants;
    using namespace Catch::Matchers;

    std::string addr;
    address_calculator calc(port_delim);
    unique_ptr<latest_pair_socket> sp1, sp2;

    REQUIRE_NOTHROW(sp1 = make_unique<latest_pair_socket>());
    REQUIRE_NOTHROW(sp2 = make_unique<latest_pair_socket>());

    REQUIRE_NOTHROW(addr = calc.get_next_addr(wildcard_addr_base));
    WARN("Listening to address: '" + addr + "'");
    REQUIRE_NOTHROW(sp1->listen(addr));

    REQUIRE_NOTHROW(addr = calc.get_addr(loopback_addr_base));
    WARN("Dialing address: '" + addr + "'");
    REQUIRE_NOTHROW(sp2->dial(addr));
}
