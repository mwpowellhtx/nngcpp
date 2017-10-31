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
#include "../catch/catch_macros.hpp"

#include "../helpers/constants.h"

#include <ostream>

struct device_fixture {

    typedef nng::protocol::latest_pair_socket latest_pair_socket;
    typedef nng::device device;

private:

    std::unique_ptr<device> devp;

public:

    device_fixture() : devp() {
    }

    virtual ~device_fixture() {
        REQUIRE(devp != nullptr);
        REQUIRE_NOTHROW(devp.reset());
        ::nng_fini();
    }

    void install(latest_pair_socket* const sp1, latest_pair_socket* const sp2, bool shouldCloseSockets) {
        REQUIRE(devp == nullptr);
        REQUIRE(sp1 != nullptr);
        REQUIRE(sp2 != nullptr);
        devp = std::make_unique<device>(sp1, sp2, shouldCloseSockets);
        REQUIRE(devp != nullptr);
    }

    bool is_installed() const {
        return devp != nullptr;
    }

    device* const get_device() {
        REQUIRE(devp != nullptr);
        return devp.get();
    }
};

namespace constants {

    const std::string dev1_addr = "inproc://dev1";
    const std::string dev2_addr = "inproc://dev2";

    const std::string alpha = "alpha";
    const std::string omega = "omega";

    const auto alpha_buf = to_buffer(alpha);
    const auto omega_buf = to_buffer(omega);
}

TEST_CASE("Test that device functions properly", "[device]") {

    using namespace std;
    using namespace std::chrono;
    using namespace nng;
    using namespace nng::protocol;
    using namespace constants;
    using namespace Catch;
    using namespace Catch::Matchers;
    using O = option_names;

    /* No need to involve any "sessions" at this level. In fact, I think that having
    a device "fixture" better focuses the unit test on just what should be vetted. */

    device_fixture fixture;

    SECTION("nng::protocol::v1::pair_socket based device functions properly") {

		SECTION("We can create a nng::protocol::v1::pair_socket device") {

            latest_pair_socket p1, p2;

            // TODO: TBD: in the grand scheme of things, I'm not really sure what purpose these two assertions are serving...
            REQUIRE_NOTHROW(p1.GetOptions()->SetInt32(O::raw, 1));
            REQUIRE_NOTHROW(p2.GetOptions()->SetInt32(O::raw, 1));

            REQUIRE_NOTHROW(fixture.install(&p1, &p2, true));
            REQUIRE(fixture.is_installed() == true);

            REQUIRE_NOTHROW(p1.listen(dev1_addr));
            REQUIRE_NOTHROW(p2.listen(dev2_addr));

            latest_pair_socket e1, e2;

            REQUIRE_NOTHROW(e1.dial(dev1_addr));
            REQUIRE_NOTHROW(e2.dial(dev2_addr));

            const auto timeout = 1000ms;

            REQUIRE_NOTHROW(e1.GetOptions()->SetDuration(O::recv_timeout_duration, timeout));
            REQUIRE_NOTHROW(e2.GetOptions()->SetDuration(O::recv_timeout_duration, timeout));

            SLEEP_FOR(100ms);

            SECTION("Device can send and receive") {

                // No need to vet "message free" at the end of this on account of message dtor.
                _Message bm;

                SECTION("Device can send") {

                    REQUIRE_NOTHROW(bm << alpha);
                    REQUIRE_NOTHROW(e1.Send(&bm));
                    REQUIRE_NOTHROW(e2.TryReceive(&bm));
                    REQUIRE_THAT(bm.GetBody()->Get(), Equals(alpha_buf));
                }

                SECTION("Device can receive") {

                    REQUIRE_NOTHROW(bm << omega);
                    REQUIRE_NOTHROW(e2.Send(&bm));
                    REQUIRE_NOTHROW(e1.TryReceive(&bm));
                    REQUIRE_THAT(bm.GetBody()->Get(), Equals(omega_buf));
                }
            }
		}
	}
}
