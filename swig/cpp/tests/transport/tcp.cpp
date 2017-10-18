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

#include <core/core.h>

#ifdef _WIN32
#   include <WinSock2.h>
#else
#   include <arpa/inet.h>
#endif

namespace nng {
    namespace transport {
        namespace v4 {

            using namespace messaging;

            void property_tests(binary_message* bmp, listener* lp, dialer* dp) {

                using namespace std;
                using namespace protocol;
                using O = option_names;

                const auto expected_family = af_inet;
                const auto loopback_addr = ::htonl(0x7f000001);

                unique_ptr<message_pipe> pp;
                unique_ptr<address> ap;

                REQUIRE_NOTHROW(pp = make_unique<message_pipe>(bmp));
                REQUIRE(pp->has_one() == true);

                REQUIRE_NOTHROW(ap = make_unique<address>());
                REQUIRE(ap->has_one() == true);

                address::size_type actual_sz = ap->get_size();

                SECTION("Local address property works") {

                    // TODO: TBD: this is really more of an unit test thing...
                    REQUIRE_NOTHROW(pp->get_option(O::local_address, ap->get(), &actual_sz));
                    REQUIRE(actual_sz == ap->get_size());
                    REQUIRE(ap->get_family() == expected_family);
                    REQUIRE(ap->view() != nullptr);
                    REQUIRE(ap->view()->get_addr() == loopback_addr);
                    REQUIRE(ap->view()->get_port() > 0);
                }

                SECTION("Remote address property works") {

                    REQUIRE_NOTHROW(pp->get_option(O::remote_address, ap->get(), &actual_sz));
                    REQUIRE(actual_sz == ap->get_size());
                    REQUIRE(ap->get_family() == expected_family);
                    REQUIRE(ap->view() != nullptr);
                    REQUIRE(ap->view()->get_addr() == loopback_addr);
                    REQUIRE(ap->view()->get_port() > 0);

                    REQUIRE(dp);
                    REQUIRE_THROWS_AS_MATCHING(dp->get_option(O::remote_address, ap->get(), &actual_sz), nng_exception, THROWS_NNG_EXCEPTION(ec_enotsup));
                }
            }
        }
    }
}

namespace constants {
    const std::string loopback_addr_base = "tcp://127.0.0.1";
    const std::string wildcard_addr_base = "tcp://*";
}

namespace nng {
    using std::placeholders::_1;
    using std::placeholders::_2;
    using std::placeholders::_3;
}

TEST_CASE("TCP transport", "[tcp][transport][nng][cxx]") {

    using namespace std;
    using namespace nng;
    using namespace messaging;
    using namespace transport;
    using namespace constants;

    const auto& property_tests = std::bind(&v4::property_tests, _1, _2, _3);

    extended_transport_fixture fixture(loopback_addr_base, property_tests);

    fixture.run_all();

    const auto cannot_connect_to_wildcards = [](address_calculator& calc) {

        using namespace protocol;
        using namespace Catch::Matchers;

        SECTION("We cannot connect to wildcards") {

            string addr;
            unique_ptr<latest_pair_socket> sp;

            REQUIRE_NOTHROW(sp = make_unique<latest_pair_socket>());

            REQUIRE_NOTHROW(addr = calc.get_next_addr(wildcard_addr_base));

            INFO("Attempting to dial address: '" + addr + "'");
            REQUIRE_THROWS_AS_MATCHING(sp->dial(addr), nng_exception, THROWS_NNG_EXCEPTION(ec_eaddrinval));
        }
    };

    const auto can_bind_to_wildcard = [](address_calculator& calc) {

        using namespace protocol;
        using namespace Catch::Matchers;

        SECTION("We can bind to wildcards") {

            string addr;
            unique_ptr<latest_pair_socket> sp1, sp2;

            REQUIRE_NOTHROW(sp1 = make_unique<latest_pair_socket>());
            REQUIRE_NOTHROW(sp2 = make_unique<latest_pair_socket>());

            REQUIRE_NOTHROW(addr = calc.get_next_addr(wildcard_addr_base));
            INFO("Listening to address: '" + addr + "'");
            REQUIRE_NOTHROW(sp1->listen(addr));

            REQUIRE_NOTHROW(addr = calc.get_addr(loopback_addr_base));
            INFO("Dialing address: '" + addr + "'");
            REQUIRE_NOTHROW(sp2->dial(addr));
        }
    };

    fixture.run_many(cannot_connect_to_wildcards, can_bind_to_wildcard);
}
