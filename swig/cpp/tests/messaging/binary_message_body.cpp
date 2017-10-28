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
#include "../helpers/constants.h"

#include "binary_message_part_fixtures.hpp"

namespace constants {

    const nng::buffer_vector_type default_data = {};

    const std::string this_is_a_test = "this is a test";
    const std::string mathematics_is_cool = "mathematics is cool";

    const auto this_is_a_test_buf = to_buffer(this_is_a_test);
    const auto mathematics_is_cool_buf = to_buffer(mathematics_is_cool);

    // Hopefully these are not too awfully contrived examples.
    const uint32_t abcd_value = ('a' << 24) | ('b' << 16) | ('c' << 8) | 'd';
    const auto abcd_value_buf = to_buffer("abcd");

    const uint32_t efgh_value = ('e' << 24) | ('f' << 16) | ('g' << 8) | 'h';
    const auto efgh_value_buf = to_buffer("efgh");

    const auto abcdefgh_value_buf = to_buffer("abcdefgh");

    const std::string mathematics_ = "mathematics ";
    const std::string _cool = " cool";
    const auto mathematics_is_buf = to_buffer("mathematics is");

    const std::string this_ = "this ";
    const std::string _a_test = " a test";
    const auto this_is_buf = to_buffer("this is");

    const auto it_is_what_it_is_buf = to_buffer("is");
}

/* TODO: TBD: I do not really see a need to re-introduce any C style unit tests for the
time being since I have a pretty good handle on proper message operation. However, that
could change downstream from now. */

TEST_CASE("Binary message part cannot exist apart from a parent message"
    , Catch::Tags("body", "part", "orphan", "invalid", "operation"
        , "messaging", "nng", "cxx").c_str()) {

    using namespace nng;
    using namespace nng::exceptions;

    /* Message part fixtured in order to simulate access to the ctor. This is
    only for test purposes and should never occur under normal operation. */

    typedef binary_message_part_fixture<binary_message_body> message_part_fixture_type;

    REQUIRE_THROWS_AS(message_part_fixture_type(nullptr), invalid_operation);
}

TEST_CASE("Verify default message part", Catch::Tags("body"
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

            binary_message_body* partp;
            REQUIRE_NOTHROW(partp = bmp->body());
            REQUIRE(partp);
            REQUIRE(partp->HasOne() == bmp->HasOne());
            REQUIRE(partp->GetSize() == 0);
            REQUIRE_THAT(partp->Get(), Equals(default_data));
        }

        SECTION("Parent message is properly reset") {
            REQUIRE_NOTHROW(bmp.reset());
        }
    }
}

TEST_CASE("Message part operates correctly", Catch::Tags("body"
    , "message", "parts", "operation", "messaging", "cxx").c_str()) {

    using namespace std;
    using namespace nng;
    using namespace constants;
    using namespace Catch::Matchers;
    using nng::uint32_t;

    SECTION("Message can be allocated properly") {

        unique_ptr<binary_message> bmp;

        REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
        REQUIRE(bmp != nullptr);
        REQUIRE(bmp->HasOne() == true);

        SECTION("Part operation") {

            binary_message_body* partp;
            REQUIRE_NOTHROW(partp = bmp->body());
            REQUIRE(partp);

            SECTION("Can append a string") {

                REQUIRE_NOTHROW(partp->Append(this_is_a_test));
                REQUIRE(partp->GetSize() == this_is_a_test.length());

                SECTION("Get is correct") {
                    REQUIRE_THAT(partp->Get(), Equals(this_is_a_test_buf));
                }

                SECTION("Right Trim is correct") {

                    REQUIRE_NOTHROW(partp->TrimRight(_a_test.length()));
                    REQUIRE_THAT(partp->Get(), Equals(this_is_buf));

                    SECTION("Left Trim is correct") {
                        REQUIRE_NOTHROW(partp->TrimLeft(this_.length()));
                        REQUIRE_THAT(partp->Get(), Equals(it_is_what_it_is_buf));
                    }
                }
            }

            SECTION("Can append a byte vector") {

                REQUIRE_NOTHROW(partp->Append(mathematics_is_cool));
                REQUIRE(partp->GetSize() == mathematics_is_cool.length());

                SECTION("Get is correct") {
                    REQUIRE_THAT(partp->Get(), Equals(mathematics_is_cool_buf));
                }

                SECTION("Right Trim is correct") {

                    REQUIRE_NOTHROW(partp->TrimRight(_cool.length()));
                    REQUIRE_THAT(partp->Get(), Equals(mathematics_is_buf));

                    SECTION("Left Trim is correct") {
                        REQUIRE_NOTHROW(partp->TrimLeft(mathematics_.length()));
                        REQUIRE_THAT(partp->Get(), Equals(it_is_what_it_is_buf));
                    }
                }
            }

            SECTION("Can append integers") {

                REQUIRE_NOTHROW(partp->Append(abcd_value));
                REQUIRE_NOTHROW(partp->Append(efgh_value));

                SECTION("Get is correct") {
                    REQUIRE_THAT(partp->Get(), Equals(abcdefgh_value_buf));
                }

                uint32_t value = 0;

                CHECK(!value);

                SECTION("Left Trim is correct") {
                    REQUIRE_NOTHROW(partp->TrimLeft(&value));
                    REQUIRE(value == abcd_value);
                    REQUIRE_THAT(partp->Get(), Equals(efgh_value_buf));
                }

                SECTION("Right Trim is correct") {
                    REQUIRE_NOTHROW(partp->TrimRight(&value));
                    REQUIRE(value == efgh_value);
                    REQUIRE_THAT(partp->Get(), Equals(abcd_value_buf));
                }
            }

            SECTION("Can prepend integers") {

                // Notice in reverse order from the append.
                REQUIRE_NOTHROW(partp->Prepend(efgh_value));
                REQUIRE_NOTHROW(partp->Prepend(abcd_value));

                SECTION("Get is correct") {
                    REQUIRE_THAT(partp->Get(), Equals(abcdefgh_value_buf));
                }

                uint32_t value = 0;

                CHECK(!value);

                SECTION("Left Trim is correct") {
                    REQUIRE_NOTHROW(partp->TrimLeft(&value));
                    REQUIRE(value == abcd_value);
                    REQUIRE_THAT(partp->Get(), Equals(efgh_value_buf));
                }

                SECTION("Right Trim is correct") {
                    REQUIRE_NOTHROW(partp->TrimRight(&value));
                    REQUIRE(value == efgh_value);
                    REQUIRE_THAT(partp->Get(), Equals(abcd_value_buf));
                }
            }
        }

        SECTION("Message is properly reset") {
            REQUIRE_NOTHROW(bmp.reset());
        }
    }
}
