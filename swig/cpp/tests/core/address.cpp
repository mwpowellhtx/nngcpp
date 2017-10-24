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

#include "../catch/catch_exception_translations.hpp"
#include "../catch/catch_tags.h"

#include "../helpers/constants.h"

#ifdef _WIN32
#   include <WinSock2.h>
#else
#   include <arpa/inet.h>
#endif

#include <type_traits>
#include <limits>
#include <map>

namespace nng {
    class address_fixture : public address {
    public:

        address_fixture() : address() {
        }

        address_fixture(const address_fixture& other) : address(other) {
        }

        virtual ~address_fixture() {
        }

        uint16_t get_family_value() const {
            return static_cast<uint16_t>(get_family());
        }

        void set_family_value(uint16_t value) {
            get()->s_un.s_family = value;
        }
    };
}

#define AF_AS_STRING(x) nng::sockaddr_family_name<x>::name

namespace nng {

    using namespace std;
    using namespace nng::exceptions;
    using namespace Catch::Matchers;

    struct address_test {

        typedef std::function<void(address_fixture* afp, const uint16_t fv)> arrange_func;

        typedef std::function<void(family_view_base* const vp, const uint16_t fv)> assert_func;

        arrange_func arrange;

        assert_func assert;
    };

    const uint16_t expected_port = 0xab;

    const family_view_base::in6_addr_vector_type expected_ipv6_addr = {
        0x01, 0x12, 0x23, 0x34, /**/ 0x45, 0x56, 0x67, 0x78,
        0x89, 0x9a, 0xab, 0xbc, /**/ 0xcd, 0xde, 0xef, 0xf0,
    };

    /* The arrange handlers may seem a bit redundant until they are more fully fleshed out,
    but leave them alone. These provide line level traceability to the specific test should
    any of them fail. */

    address_test max_fv_tests{
        [](address_fixture* afp, const uint16_t fv) {
        auto vp = afp->view();
        REQUIRE_THROWS_AS(vp->set_port(0), not_implemented);
        REQUIRE_THROWS_AS(vp->set_addr(0), not_implemented);
        REQUIRE_THROWS_AS(vp->set_in6_addr({}), not_implemented);
        REQUIRE_THROWS_AS(vp->set_in6_addr16({}), not_implemented);
        REQUIRE_THROWS_AS(vp->set_in6_addr32({}), not_implemented);
    }
        , [](family_view_base* const vp, const uint16_t fv) {
        // In this case Family and Jewel should not agree.
        REQUIRE(vp->get_jewel() == af_unspec);
        REQUIRE(vp->get_family() == numeric_limits<uint16_t>::max());
        REQUIRE_THROWS_AS(vp->get_addr(), not_implemented);
        REQUIRE_THROWS_AS(vp->get_in6_addr(), not_implemented);
        REQUIRE_THROWS_AS(vp->get_in6_addr16(), not_implemented);
        REQUIRE_THROWS_AS(vp->get_in6_addr32(), not_implemented);
        REQUIRE_THROWS_AS(vp->get_port(), not_implemented);
    }
    };

    // TODO: TBD: may also introduce a "no arrangement" baseline; outcomes should be aligned with UNSPEC, I believe.
    address_test unspec_tests{
        [](address_fixture* afp, const uint16_t fv) {
        auto vp = afp->view();
        REQUIRE_THROWS_AS(vp->set_port(0), not_implemented);
        REQUIRE_THROWS_AS(vp->set_addr(0), not_implemented);
        REQUIRE_THROWS_AS(vp->set_in6_addr({}), not_implemented);
        REQUIRE_THROWS_AS(vp->set_in6_addr16({}), not_implemented);
        REQUIRE_THROWS_AS(vp->set_in6_addr32({}), not_implemented);
    }
        , [](family_view_base* const vp, const uint16_t fv) {
        REQUIRE(vp->get_jewel() == af_unspec);
        REQUIRE(vp->get_family() == vp->get_jewel());
        REQUIRE_THROWS_AS(vp->get_addr(), not_implemented);
        REQUIRE_THROWS_AS(vp->get_in6_addr(), not_implemented);
        REQUIRE_THROWS_AS(vp->get_in6_addr16(), not_implemented);
        REQUIRE_THROWS_AS(vp->get_in6_addr32(), not_implemented);
        REQUIRE_THROWS_AS(vp->get_port(), not_implemented);
    }
    };

    address_test inproc_tests{
        [](address_fixture* afp, const uint16_t fv) {
        auto vp = afp->view();
        REQUIRE_THROWS_AS(vp->set_port(0), not_implemented);
        REQUIRE_THROWS_AS(vp->set_addr(0), not_implemented);
        REQUIRE_THROWS_AS(vp->set_in6_addr({}), not_implemented);
        REQUIRE_THROWS_AS(vp->set_in6_addr16({}), not_implemented);
        REQUIRE_THROWS_AS(vp->set_in6_addr32({}), not_implemented);
    }
        , [](family_view_base* const vp, const uint16_t fv) {
        REQUIRE(vp->get_jewel() == af_inproc);
        REQUIRE(vp->get_family() == vp->get_jewel());
        REQUIRE(dynamic_cast<inproc_family_view*>(vp));
        REQUIRE_THROWS_AS(vp->get_addr(), not_implemented);
        REQUIRE_THROWS_AS(vp->get_in6_addr(), not_implemented);
        REQUIRE_THROWS_AS(vp->get_in6_addr16(), not_implemented);
        REQUIRE_THROWS_AS(vp->get_in6_addr32(), not_implemented);
        REQUIRE_THROWS_AS(vp->get_port(), not_implemented);
    }
    };

    address_test ipc_tests{
        [](address_fixture* afp, const uint16_t fv) {
        auto vp = afp->view();
        REQUIRE_THROWS_AS(vp->set_port(0), not_implemented);
        REQUIRE_THROWS_AS(vp->set_addr(0), not_implemented);
        REQUIRE_THROWS_AS(vp->set_in6_addr({}), not_implemented);
        REQUIRE_THROWS_AS(vp->set_in6_addr16({}), not_implemented);
        REQUIRE_THROWS_AS(vp->set_in6_addr32({}), not_implemented);
    }
        , [](family_view_base* const vp, const uint16_t fv) {
        REQUIRE(vp->get_jewel() == af_ipc);
        REQUIRE(vp->get_family() == vp->get_jewel());
        REQUIRE(dynamic_cast<ipc_family_view*>(vp));
        REQUIRE_THROWS_AS(vp->get_addr(), not_implemented);
        REQUIRE_THROWS_AS(vp->get_in6_addr(), not_implemented);
        REQUIRE_THROWS_AS(vp->get_in6_addr16(), not_implemented);
        REQUIRE_THROWS_AS(vp->get_in6_addr32(), not_implemented);
        REQUIRE_THROWS_AS(vp->get_port(), not_implemented);
    }
    };

    address_test inet_tests{
        [](address_fixture* afp, const uint16_t fv) {
        auto vp = dynamic_cast<inet_family_view*>(afp->view());
        REQUIRE(vp);
        REQUIRE(!vp->get_port());
        REQUIRE(!vp->get_addr());
        REQUIRE_THROWS_AS(vp->set_in6_addr({}), not_implemented);
        REQUIRE_THROWS_AS(vp->set_in6_addr16({}), not_implemented);
        REQUIRE_THROWS_AS(vp->set_in6_addr32({}), not_implemented);
        REQUIRE_NOTHROW(vp->set_port(expected_port));
        REQUIRE_NOTHROW(vp->set_addr(INADDR_LOOPBACK));
    },
        [](family_view_base* const vp, const uint16_t fv) {
        REQUIRE(vp->get_jewel() == af_inet);
        REQUIRE(vp->get_family() == vp->get_jewel());
        REQUIRE_THROWS_AS(vp->get_in6_addr(), not_implemented);
        REQUIRE_THROWS_AS(vp->get_in6_addr16(), not_implemented);
        REQUIRE_THROWS_AS(vp->get_in6_addr32(), not_implemented);
        auto vp2 = dynamic_cast<inet_family_view*>(vp);
        REQUIRE(vp2);
        REQUIRE(vp2->get_addr() == INADDR_LOOPBACK);
        REQUIRE(vp2->get_port() == expected_port);
    }
    };

    address_test inet6_tests{
        [](address_fixture* afp, const uint16_t fv) {
        auto vp = dynamic_cast<inet6_family_view*>(afp->view());
        REQUIRE(vp);
        REQUIRE_THROWS_AS(vp->set_addr(0), not_implemented);
        REQUIRE(!vp->get_port());
        // TODO: TBD: https://en.wikipedia.org/wiki/IPv6 / https://en.wikipedia.org/wiki/IPv6_address
        // TODO: TBD: NNG IPv6 representation not ready for prime time yet, I think it needs a little work in the address representation
        // https://github.com/nanomsg/nng/issues/119
        REQUIRE_NOTHROW(vp->set_port(expected_port));
        REQUIRE_NOTHROW(vp->set_in6_addr(expected_ipv6_addr));
        // TODO: TBD: these should be implemented one way or another...
        CHECK_THROWS_AS(vp->set_in6_addr16({}), not_implemented);
        CHECK_THROWS_AS(vp->set_in6_addr32({}), not_implemented);
    },
        [](family_view_base* const vp, const uint16_t fv) {
        REQUIRE(vp->get_jewel() == af_inet6);
        REQUIRE(vp->get_family() == vp->get_jewel());
        REQUIRE_THROWS_AS(vp->get_addr(), not_implemented);
        REQUIRE(dynamic_cast<inet6_family_view*>(vp));
        REQUIRE_NOTHROW(vp->get_in6_addr());
        // TODO: TBD: ditto the arrange/implementation...
        CHECK_THROWS_AS(vp->get_in6_addr16(), not_implemented);
        CHECK_THROWS_AS(vp->get_in6_addr32(), not_implemented);
        REQUIRE_NOTHROW(vp->get_port());
    }
    };

    address_test zt_tests{
        [](address_fixture* afp, const uint16_t fv) {
        auto vp = afp->view();
        REQUIRE_THROWS_AS(vp->set_port(0), not_implemented);
        REQUIRE_THROWS_AS(vp->set_addr(0), not_implemented);
        REQUIRE_THROWS_AS(vp->set_in6_addr({}), not_implemented);
        REQUIRE_THROWS_AS(vp->set_in6_addr16({}), not_implemented);
        REQUIRE_THROWS_AS(vp->set_in6_addr32({}), not_implemented);
    },
        [](family_view_base* const vp, const uint16_t fv) {
        REQUIRE(vp->get_jewel() == af_zt);
        REQUIRE(vp->get_family() == vp->get_jewel());
        REQUIRE(dynamic_cast<zt_family_view*>(vp));
        REQUIRE_THROWS_AS(vp->get_addr(), not_implemented);
        REQUIRE_THROWS_AS(vp->get_in6_addr(), not_implemented);
        REQUIRE_THROWS_AS(vp->get_in6_addr16(), not_implemented);
        REQUIRE_THROWS_AS(vp->get_in6_addr32(), not_implemented);
        REQUIRE_THROWS_AS(vp->get_port(), not_implemented);
    }
    };
}

TEST_CASE("Tests that socket addresses are handled correctly", Catch::Tags(
    "address", "socket", "cxx").c_str()) {

    using namespace std;
    using namespace nng;
    using namespace nng::exceptions;
    using namespace Catch::Matchers;

    address_fixture af;
    family_view_base* vp1 = nullptr;

    const uint16_t default_fv = 0;
    const uint16_t max_fv = numeric_limits<uint16_t>::max();

    SECTION("Address family as string are all correct") {

        // No need to test extraneous cases because the template specialization takes care of itself.
        REQUIRE_THAT(AF_AS_STRING(af_unspec), Equals("af_unspec"));
        REQUIRE_THAT(AF_AS_STRING(af_inproc), Equals("af_inproc"));
        REQUIRE_THAT(AF_AS_STRING(af_ipc), Equals("af_ipc"));
        REQUIRE_THAT(AF_AS_STRING(af_inet), Equals("af_inet"));
        REQUIRE_THAT(AF_AS_STRING(af_inet6), Equals("af_inet6"));
        REQUIRE_THAT(AF_AS_STRING(af_zt), Equals("af_zt"));
    }

    SECTION("Prepare to verify each of the views") {

        typedef map<uint16_t, address_test* const> address_test_map;

        address_test_map tests = {
            { af_unspec, &unspec_tests },
            { af_inproc, &inproc_tests },
            { af_ipc, &ipc_tests },
            { af_inet, &inet_tests },
            { af_inet6, &inet6_tests },
            { af_zt, &zt_tests },
            { max_fv, &max_fv_tests },
        };

        for_each(tests.begin(), tests.end(), [&af](address_test_map::value_type& args) {

            // Really for use with the socket address type.
            uint16_t fv = args.first;

            SECTION("Verify address for family value: '" + address::name_of(fv) + "'") {

                address_test* const testsp = args.second;

                REQUIRE_NOTHROW(af.set_family_value(fv));

                testsp->arrange(&af, fv);

                family_view_base* const vp = af.view();
                REQUIRE(vp);
                testsp->assert(vp, fv);
            }
        });
    }

    SECTION("Preparing to verify view transitions") {

        typedef multimap<sockaddr_family_type, sockaddr_family_type> transitions_map;

        transitions_map transitions;

        vector<sockaddr_family_type> values = { af_unspec, af_inproc, af_ipc, af_inet, af_inet6, af_zt };

        auto _values_begin = values.begin();
        auto _values_end = values.end();

        for (auto xt = _values_begin; xt != _values_end; ++xt) {
            for (auto yt = _values_begin; yt != _values_end; ++yt) {
                if (*xt == *yt) { continue; }
                transitions.emplace(*xt, *yt);
            }
        }

        auto previous_vp = af.view();

        REQUIRE(previous_vp);

        SECTION("Verify that view transitions can occur seamlessly") {

            const auto __verify = [&af, &previous_vp](const sockaddr_family_type x) {
                const auto y = af.get_family();
                if (x == y) { return; }
                INFO("Transitioning from '" + address::name_of(y)
                    + "' to '" + address::name_of(x) + "'");
                REQUIRE_NOTHROW(af.set_family(x));
                family_view_base* vp;
                REQUIRE_NOTHROW(vp = af.view());
                REQUIRE(vp);
                REQUIRE(vp->get_jewel() == x);
                // The View itself looks okay, but did it really change?
                REQUIRE(vp != previous_vp);
                // And hold on to the Previous View Pointer for comparison.
                previous_vp = vp;
            };

            for (auto it = transitions.begin(); it != transitions.end(); ++it) {
                __verify(it->first);
                __verify(it->second);
            }
        }
    }
}
