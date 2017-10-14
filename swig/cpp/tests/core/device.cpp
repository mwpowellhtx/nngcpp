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

#include "../catch/catch_exception_translations.hpp"
#include "../helpers/chrono.hpp"

#include <nngcpp.h>

#include <string>
#include <chrono>
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
    const nng::messaging::message_base::buffer_vector_type alpha_buf = { 'a','l','p','h','a' };
    const nng::messaging::message_base::buffer_vector_type omega_buf = { 'o','m','e','g','a' };
}

TEST_CASE("Test that device functions properly", "[device]") {

    using namespace Catch;
    using namespace Catch::Matchers;
    using namespace std;
    using namespace std::chrono;
    using namespace nng;
    using namespace nng::protocol;
    using namespace nng::messaging;
    using namespace constants;
    using _opt_ = option_names;

    /* No need to involve any "sessions" at this level. In fact, I think that having
    a device "fixture" better focuses the unit test on just what should be vetted. */

    device_fixture fixture;

    SECTION("nng::protocol::v1::pair_socket based device functions properly") {

		SECTION("We can create a nng::protocol::v1::pair_socket device") {

            latest_pair_socket p1, p2;

            // TODO: TBD: in the grand scheme of things, I'm not really sure what purpose these two assertions are serving...
            REQUIRE_NOTHROW(p1.set_option_int(_opt_::raw, 1));
            REQUIRE_NOTHROW(p2.set_option_int(_opt_::raw, 1));

            REQUIRE_NOTHROW(fixture.install(&p1, &p2, true));
            REQUIRE(fixture.is_installed() == true);

            REQUIRE_NOTHROW(p1.listen(dev1_addr));
            REQUIRE_NOTHROW(p2.listen(dev2_addr));

            latest_pair_socket e1, e2;

            REQUIRE_NOTHROW(e1.dial(dev1_addr));
            REQUIRE_NOTHROW(e2.dial(dev2_addr));

            const auto timeout = 1000ms;

            REQUIRE_NOTHROW(e1.set_option_usec(_opt_::receive_timeout_usec, CAST_DURATION_TO_USEC(timeout).count()));
            REQUIRE_NOTHROW(e2.set_option_usec(_opt_::receive_timeout_usec, CAST_DURATION_TO_USEC(timeout).count()));

            this_thread::sleep_for(100ms);

            SECTION("Device can send and receive") {

                // No need to vet "message free" at the end of this on account of message dtor.
                binary_message bm;

                SECTION("Device can send") {

                    REQUIRE_NOTHROW(bm << alpha);
                    REQUIRE_NOTHROW(e1.send(&bm));
                    REQUIRE_NOTHROW(e2.try_receive(&bm));
                    REQUIRE_THAT(bm.body()->get(), Equals(alpha_buf));
                }

                SECTION("Device can receive") {

                    REQUIRE_NOTHROW(bm << omega);
                    REQUIRE_NOTHROW(e2.send(&bm));
                    REQUIRE_NOTHROW(e1.try_receive(&bm));
                    REQUIRE_THAT(bm.body()->get(), Equals(omega_buf));
                }
            }
		}
	}
}
