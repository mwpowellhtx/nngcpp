//
// Copyright (c) 2017 Michel W Powell <mwpowellhtx@gmail.com>
//
// This software is supplied under the terms of the MIT License, a
// copy of which should be located in the distribution where this
// file was obtained (LICENSE.txt).  A copy of the license may also be
// found online at https://opensource.org/licenses/MIT.
//

#include <nngcpp.h>

#include "../catch/catch_exception_translations.hpp"
#include "../catch/catch_exception_matcher_base.hpp"
#include "../catch/catch_nng_exception_matcher.hpp"
#include "../catch/catch_tags.h"

#include "binary_message_part_fixtures.hpp"

namespace constants {
    const nng::messaging::buffer_vector_type default_data = {};
}

/* TODO: TBD: Ditto notes concerning body message part. */

TEST_CASE("Binary message part cannot exist apart from a parent message"
    , Catch::Tags("header", "part", "orphan", "invalid", "operation"
        , "messaging", "nng", "cxx").c_str()) {

    using namespace trx;
    using namespace nng::messaging;

    /* Message part fixtured in order to simulate access to the ctor. This is
    only for test purposes and should never occur under normal operation. */

    typedef binary_message_part_fixture<binary_message_header> message_part_fixture_type;

    REQUIRE_THROWS_AS(message_part_fixture_type(nullptr), invalid_operation);
}

TEST_CASE("Verify default message part", Catch::Tags("header"
    , "default", "part", "messaging", "nng", "cxx").c_str()) {

    using namespace std;
    using namespace nng::messaging;
    using namespace constants;
    using namespace Catch::Matchers;

    SECTION("Parent message can be allocated properly") {

        unique_ptr<binary_message> bmp;

        REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
        REQUIRE(bmp != nullptr);
        REQUIRE(bmp->has_one() == true);

        SECTION("Message part can be obtained from parent") {

            binary_message_header* headerp;
            REQUIRE_NOTHROW(headerp = bmp->header());
            REQUIRE(headerp);
            REQUIRE(headerp->has_one() == bmp->has_one());
            REQUIRE(headerp->get_size() == 0);
            REQUIRE_THAT(headerp->get(), Equals(default_data));
        }

        SECTION("Parent message is properly reset") {
            REQUIRE_NOTHROW(bmp.reset());
        }
    }
}
