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

#include <nngcpp.h>

#include "extended_transport.h"

#ifdef __cplusplus
extern "C" {
#   include <core/nng_impl.h>
}
#endif // __cplusplus

namespace constants {
    // The IPv6 loopback is represented by "::1" and the [] brackets represents the ability to append a ":<port>".
    const std::string loopback_addr_base = "tcp://[::1]";
    const std::string test_addr_base = loopback_addr_base;
    const char port_delim = ':';
}

namespace nng {

    namespace transport {

        // TCP tests for IPv6.
        bool has_v6() {
            /* The Udp is for internal use. We do not need to know for public
            consumption. But we do use it here in the unit testing. */
            ::nni_plat_udp* udpp = nullptr;
            auto loopback = address::in6_loopback();
            // TODO: TBD: so the only reason why we do this is to back-support the NNG structure.
            ::nng_sockaddr sa;
            std::memcpy(&sa, loopback.get(), sizeof(sa));
            int rv;
            // A call like this works because the nng_sockaddr is type-defined as nni_sockaddr.
            CHECK_NOFAIL((rv = ::nni_plat_udp_open(&udpp, &sa)) == 0);
            if (rv == 0) {
                ::nni_plat_udp_close(udpp);
            }
            return !rv;
        }
    }
}

void init(const std::string& addr) {
    // TODO: TBD: what kind of initialization do we need here?
}

namespace nng {
    namespace messaging {

        const family_view_base::in6_addr_vector_type expected_loopback_long_addr
            = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 };

        const family_view_base::in6_addr_vector_type expected_loopback_short_addr = { 1 };

        using O = option_names;

        template<>
        void test_local_addr_properties<::nng_pipe, ::nng_listener, ::nng_dialer>(::nng_pipe* const pp
            , ::nng_listener* const lp, ::nng_dialer* const dp, uint16_t expected_port) {

            using namespace std;

            ::nng_sockaddr a;
            size_type sz = sizeof(a);

            REQUIRE(::nng_pipe_getopt(*pp, O::local_address.c_str(), (void*)&a, &sz) == 0);
            REQUIRE(sz == sizeof(a));
            REQUIRE(a.s_un.s_family == ::NNG_AF_INET6);
            REQUIRE(memcmp(&(a.s_un.s_in6.sa_addr), &expected_loopback_long_addr[0], 16) == 0);
            REQUIRE(a.s_un.s_in6.sa_port == ::htons(expected_port));
        }

        template<>
        void test_remote_addr_properties<::nng_pipe, ::nng_listener, ::nng_dialer>(::nng_pipe* const pp
            , ::nng_listener* const lp, ::nng_dialer* const dp, uint16_t expected_port) {

            using namespace std;

            ::nng_sockaddr a;
            size_type sz = sizeof(a);

            REQUIRE(::nng_pipe_getopt(*pp, O::remote_address.c_str(), (void*)&a, &sz) == 0);
            REQUIRE(sz == sizeof(a));
            REQUIRE(a.s_un.s_family == ::NNG_AF_INET6);
            REQUIRE(memcmp(&(a.s_un.s_in6.sa_addr), &expected_loopback_long_addr[0], 16) == 0);
            REQUIRE(a.s_un.s_in.sa_port != 0);
        }

        template<>
        void test_local_addr_properties<message_pipe, listener, dialer>(message_pipe* const pp
            , listener* const lp, dialer* const dp, uint16_t expected_port) {

            using namespace Catch::Matchers;

            // TODO: TBD: call address socket_address instead... would be more specific.
            address a;

            REQUIRE_NOTHROW(pp->get_option(O::local_address, &a));
            REQUIRE(a.get_family() == af_inet6);
            auto vp = a.view();
            REQUIRE(vp->get_family() == a.get_family());
            REQUIRE_THAT(vp->get_in6_addr(), Equals(expected_loopback_short_addr));
            REQUIRE(vp->get_port() == expected_port);
        }

        template<>
        void test_remote_addr_properties<message_pipe, listener, dialer>(message_pipe* const pp
            , listener* const lp, dialer* const dp, uint16_t expected_port) {

            using namespace Catch::Matchers;

            // TODO: TBD: call address socket_address instead... would be more specific.
            address a;

            REQUIRE_NOTHROW(pp->get_option(O::remote_address, &a));
            REQUIRE(a.get_family() == af_inet6);
            auto vp = a.view();
            REQUIRE(vp->get_family() == a.get_family());
            REQUIRE_THAT(vp->get_in6_addr(), Equals(expected_loopback_short_addr));
            REQUIRE(vp->get_port() != 0);
        }
    }
}
