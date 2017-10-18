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

#include <core/nng_impl.h>

namespace nng {

    // TCP tests for IPv6.
    bool has_v6() {

        ::nng_sockaddr sa;
        ::nni_plat_udp* udpp;

        sa.s_un.s_in6.sa_family = ::NNG_AF_INET6;
        sa.s_un.s_in6.sa_port = 0;
        memset(sa.s_un.s_in6.sa_addr, 0, 16);
        sa.s_un.s_in6.sa_addr[15] = 1;

        int rv;
        rv = ::nni_plat_udp_open(&udpp, &sa);
        CHECK_NOFAIL(!rv);
        if (rv == 0) {
            ::nni_plat_udp_close(udpp);
        }

        return !rv;
    }

    namespace transport {
        namespace v6 {

            using namespace messaging;

            // was: static int check_props_v6(nng_msg *msg, nng_listener l, nng_dialer d)

            void property_tests(binary_message *bmp, listener* lp, dialer* dp) {

                using namespace std;
                using O = option_names;

                unique_ptr<address> ap;
                unique_ptr<message_pipe> pp;

                REQUIRE_NOTHROW(pp = make_unique<message_pipe>(bmp));
                REQUIRE(pp->has_one() == true);

                REQUIRE_NOTHROW(ap = make_unique<address>());

                address::size_type actual_sz = ap->get_size();



                uint8_t loopback[16];

                memset(loopback, 0, sizeof(loopback));
                loopback[15] = 1;



                SECTION("IPv6 Local address property works") {

                    REQUIRE_NOTHROW(pp->get_option(O::local_address, ap->get(), &actual_sz));
                    REQUIRE(actual_sz == ap->get_size());
                    REQUIRE(ap->get_s_family() == af_inet6);
                    REQUIRE(ap->get_port() != 0);
                    nng_sockaddr sa;
                    sa.s_un.s_in6.
                    So(memcmp(la.s_un.s_in6.sa_addr, loopback, 16) == 0);
                }

                SECTION("IPv6 remote address property works") {
                    So(nng_pipe_getopt(p, NNG_OPT_REMADDR, &ra, &z) == 0);
                    So(z == sizeof(ra));
                    So(ra.s_un.s_family == NNG_AF_INET6);
                    So(ra.s_un.s_in6.sa_port != 0);
                    So(memcmp(ra.s_un.s_in6.sa_addr, loopback, 16) == 0);

                    So(nng_dialer_getopt(d, NNG_OPT_REMADDR, &ra, &z) != 0);
                }
            }
        }
    }
}

TEST_CASE("TCP (IPv6) transport tests", "[tcp][ipv6][transport][nng][cxx]") {

    using namespace nng;

    REQUIRE(::nni_init() == 0);

	if (has_v6()) {
		trantest_test_extended("tcp://[::1]:%u", check_props_v6);
	} else {
        INFO("IPv6 not available for testing.");
        REQUIRE(false);
	}

	nng_fini();
}
