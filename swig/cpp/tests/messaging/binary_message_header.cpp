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

    const nng::buffer_vector_type default_data = {};

    const std::string __empty = "";

    const auto __empty_buf = default_data;


    const uint32_t value = 0x15263748;
    const uint32_t reversed = 0x48372615;

    const nng::buffer_vector_type appended_data = { 0x15,0x26,0x37,0x48,0x48,0x37,0x26,0x15,0x15,0x26,0x37,0x48 };

    const nng::buffer_vector_type remaining_after_ltrim = { 0x48,0x37,0x26,0x15,0x15,0x26,0x37,0x48 };

    const nng::buffer_vector_type remaining_after_rtrim = { 0x48,0x37,0x26,0x15 };
}

/* TODO: TBD: Ditto notes concerning body message part. */

TEST_CASE("Binary message part cannot exist apart from a parent message"
    , Catch::Tags("header", "part", "orphan", "invalid", "operation"
        , "messaging", "nng", "cxx").c_str()) {

    using namespace nng;
    using namespace nng::exceptions;

    /* Message part fixtured in order to simulate access to the ctor. This is
    only for test purposes and should never occur under normal operation. */

    typedef binary_message_part_fixture<binary_message_header> message_part_fixture_type;

    REQUIRE_THROWS_AS(message_part_fixture_type(nullptr), invalid_operation);
}

TEST_CASE("Verify default message part", Catch::Tags("header"
    , "default", "part", "messaging", "nng", "cxx").c_str()) {

    using namespace std;
    using namespace nng;
    using namespace constants;
    using namespace Catch::Matchers;

    SECTION("Parent message can be allocated properly") {

        unique_ptr<binary_message> bmp;

        REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
        REQUIRE(bmp != nullptr);
        REQUIRE(bmp->HasOne() == true);

        SECTION("Message part can be obtained from parent") {

            binary_message_header* headerp;
            REQUIRE_NOTHROW(headerp = bmp->header());
            REQUIRE(headerp);
            REQUIRE(headerp->HasOne() == bmp->HasOne());
            REQUIRE(headerp->get_size() == 0);
            REQUIRE_THAT(headerp->get(), Equals(default_data));
        }

        SECTION("Parent message is properly reset") {
            REQUIRE_NOTHROW(bmp.reset());
        }
    }
}

namespace nng {

    typedef binary_message_part_fixture<binary_message_header> binary_message_header_fixture;

    // TODO: TBD: any reason to define a full on class derivation? perhaps for SWIG purposes? or just type-define it?
    class binary_message_fixture : public basic_binary_message<binary_message_body, binary_message_header_fixture> {
    public:

        binary_message_fixture() : basic_binary_message() {}

        virtual ~binary_message_fixture() {}
    };
}

TEST_CASE("Message part operates correctly", Catch::Tags("header"
    , "message", "parts", "operation", "messaging", "cxx").c_str()) {

    using namespace std;
    using namespace nng;
    using namespace nng::exceptions;
    using namespace constants;
    using namespace Catch::Matchers;
    using nng::uint32_t;

    SECTION("Message can be allocated properly") {

        unique_ptr<binary_message_fixture> bmp;

        REQUIRE_NOTHROW(bmp = make_unique<binary_message_fixture>());

        SECTION("Part operation") {

            binary_message_header_fixture* partp;

            REQUIRE_NOTHROW(partp = bmp->header());
            REQUIRE(partp);

            SECTION("Appending a string throws an exception") {
                // TODO: TBD: may want to throw stronger than "not implemented", i.e. "invalid_operation"
                REQUIRE_THROWS_AS(partp->Append(__empty), not_implemented);
            }

            SECTION("Appending a byte vector throws an exception") {
                REQUIRE_THROWS_AS(partp->Append(__empty_buf), not_implemented);
            }

            size_type sz = 0;

            SECTION("Left Trim size_t throws an exception") {
                REQUIRE_THROWS_AS(partp->TrimLeft(sz), not_implemented);
            }

            SECTION("Right Trim size_t throws an exception") {
                REQUIRE_THROWS_AS(partp->TrimRight(sz), not_implemented);
            }

            // TODO: TBD: for now proceeding as though the 32-bit integer support is required.
            SECTION("Should support 32-bit integers") {

                REQUIRE_NOTHROW(partp->Append(reversed));
                REQUIRE_NOTHROW(partp->Prepend(value));
                REQUIRE_NOTHROW(partp->Append(value));
                REQUIRE_THAT(partp->get(), Equals(appended_data));

                uint32_t trimmed = 0;
                CHECK(!trimmed);

                SECTION("Should support Left Trim of a 32-bit integer") {

                    REQUIRE_NOTHROW(partp->TrimLeft(trimmed));
                    REQUIRE(trimmed == value);
                    REQUIRE_THAT(partp->get(), Equals(remaining_after_ltrim));

                    SECTION("Should also support Right Trim of a 32-bit integer") {

                        REQUIRE_NOTHROW(partp->TrimRight(trimmed));
                        REQUIRE(trimmed == value);
                        REQUIRE_THAT(partp->get(), Equals(remaining_after_rtrim));

                        SECTION("One final 32-bit integer Trim") {

                            REQUIRE_NOTHROW(partp->TrimLeft(trimmed));
                            REQUIRE(trimmed == reversed);
                            REQUIRE_THAT(partp->get(), Equals(__empty_buf));
                        }
                    }
                }
            }
        }
    }
}
