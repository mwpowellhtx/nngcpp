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

        namespace v6 {

            using namespace std;
            using namespace messaging;
            using namespace Catch::Matchers;

            // was: static int check_props_v6(nng_msg *msg, nng_listener l, nng_dialer d)

            void property_tests(binary_message *bmp, listener* lp, dialer* dp) {

                using O = option_names;

                unique_ptr<address> ap;
                unique_ptr<message_pipe> pp;

                family_view_base* actual_vp;

                REQUIRE_NOTHROW(ap = make_unique<address>());
                REQUIRE_NOTHROW(pp = make_unique<message_pipe>(bmp));
                REQUIRE(pp->has_one() == true);

                const auto in6_loopback = address::in6_loopback();
                const auto expected_vp = const_cast<address&>(in6_loopback).view();
                REQUIRE(expected_vp);

                auto actual_sz = ap->get_size();
                auto _ap_getp = ap->get();
                REQUIRE(_ap_getp);

                // TODO: TBD: it would be better to have visibility into the Fixture, the current Address, Port, etc...
                SECTION("IPv6 Local address option works") {

                    REQUIRE_NOTHROW(pp->get_option(O::local_address, _ap_getp, &actual_sz));
                    REQUIRE_NOTHROW(actual_vp = ap->view());
                    REQUIRE(actual_vp);
                    // The very fact that this step PASSES means that we had an IPv6 (inet6) View.
                    REQUIRE_THAT(actual_vp->get_in6_addr(), Equals(expected_vp->get_in6_addr()));
                    // In this case all we can know is that the Port should be set.
                    REQUIRE(actual_vp->get_port());
                }

                SECTION("IPv6 Remote address option works") {

                    REQUIRE_NOTHROW(pp->get_option(O::remote_address, _ap_getp, &actual_sz));
                    REQUIRE_NOTHROW(actual_vp = ap->view());
                    REQUIRE(actual_vp);
                    // The very fact that this step PASSES means that we had an IPv6 (inet6) View.
                    REQUIRE_THAT(actual_vp->get_in6_addr(), Equals(expected_vp->get_in6_addr()));
                    // In this case all we can know is that the Port should be set.
                    REQUIRE(actual_vp->get_port());

                    REQUIRE_NOTHROW(actual_sz = ap->get_size());
                    REQUIRE(dp);
                    REQUIRE_THROWS_AS_MATCHING(dp->get_option(O::remote_address, _ap_getp, &actual_sz), nng_exception, THROWS_NNG_EXCEPTION(ec_enotsup));
                }
            }
        }
    }
}

namespace constants {
    // The IPv6 loopback is represented by "::1" and the [] brackets represents the ability to append a ":<port>".
    const std::string loopback_addr_base = "tcp://[::1]";
}

TEST_CASE("TCP (IPv6) transport tests", "[tcp][ipv6][transport][nng][cxx]") {

    using namespace nng;
    using namespace nng::transport;
    using namespace constants;

    // TODO: TBD: do we really need to invoke ::nni_init? and for what reason? for has_v6 to work properly?
    REQUIRE(::nni_init() == 0);

    using std::placeholders::_1;
    using std::placeholders::_2;
    using std::placeholders::_3;

    if (has_v6()) {
        const auto& property_tests = std::bind(&v6::property_tests, _1, _2, _3);
        extended_transport_fixture fixture(loopback_addr_base, property_tests);
        fixture.run_all();
    } else {
        INFO("IPv6 not available for testing.");
	}

	//::nng_fini();
}
