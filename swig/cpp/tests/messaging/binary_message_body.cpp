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

    const nng::messaging::buffer_vector_type default_data = {};

    const std::string this_is_a_test = "this is a test";
    const std::string mathematics_is_cool = "mathematics is cool";

    const auto this_is_a_test_buf = to_buffer(this_is_a_test);
    const auto mathematics_is_cool_buf = to_buffer(mathematics_is_cool);

    // Hopefully these are not too awfully contrived examples.
    const uint32_t abcd_value = ('a' << 24) | ('b' << 16) | ('c' << 8) | 'd';
    const nng::messaging::buffer_vector_type abcd_value_buf = { 'a','b','c','d' };

    const uint32_t efgh_value = ('e' << 24) | ('f' << 16) | ('g' << 8) | 'h';
    const nng::messaging::buffer_vector_type efgh_value_buf = { 'e','f','g','h' };

    const nng::messaging::buffer_vector_type abcdefgh_value_buf = { 'a','b','c','d','e','f','g','h' };

    const std::string mathematics_ = "mathematics ";
    const std::string _cool = " cool";
    const auto mathematics_is_buf = to_buffer("mathematics is");

    const std::string this_ = "this ";
    const std::string _a_test = " a test";
    const auto this_is_buf = to_buffer("this is");

    const nng::messaging::buffer_vector_type it_is_what_it_is_buf = { 'i','s' };
}

/* TODO: TBD: I do not really see a need to re-introduce any C style unit tests for the
time being since I have a pretty good handle on proper message operation. However, that
could change downstream from now. */

TEST_CASE("Binary message part cannot exist apart from a parent message"
    , Catch::Tags("body", "part", "orphan", "invalid", "operation"
        , "messaging", "nng", "cxx").c_str()) {

    using namespace nng::messaging;
    using namespace nng::exceptions;

    /* Message part fixtured in order to simulate access to the ctor. This is
    only for test purposes and should never occur under normal operation. */

    typedef binary_message_part_fixture<binary_message_body> message_part_fixture_type;

    REQUIRE_THROWS_AS(message_part_fixture_type(nullptr), invalid_operation);
}

TEST_CASE("Verify default message part", Catch::Tags("body"
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

            binary_message_body* partp;
            REQUIRE_NOTHROW(partp = bmp->body());
            REQUIRE(partp);
            REQUIRE(partp->has_one() == bmp->has_one());
            REQUIRE(partp->get_size() == 0);
            REQUIRE_THAT(partp->get(), Equals(default_data));
        }

        SECTION("Parent message is properly reset") {
            REQUIRE_NOTHROW(bmp.reset());
        }
    }
}

TEST_CASE("Message part operates correctly") {

    using namespace std;
    using namespace nng::messaging;
    using namespace constants;
    using namespace Catch::Matchers;

    SECTION("Message can be allocated properly") {

        unique_ptr<binary_message> bmp;

        REQUIRE_NOTHROW(bmp = make_unique<binary_message>());
        REQUIRE(bmp != nullptr);
        REQUIRE(bmp->has_one() == true);

        SECTION("Part operation") {

            binary_message_body* partp;
            REQUIRE_NOTHROW(partp = bmp->body());
            REQUIRE(partp);

            SECTION("Can append a string") {

                REQUIRE_NOTHROW(partp->append(this_is_a_test));
                REQUIRE(partp->get_size() == this_is_a_test.length());

                SECTION("Get is correct") {
                    REQUIRE_THAT(partp->get(), Equals(this_is_a_test_buf));
                }

                SECTION("Right Trim is correct") {

                    REQUIRE_NOTHROW(partp->rtrim(_a_test.length()));
                    REQUIRE_THAT(partp->get(), Equals(this_is_buf));

                    SECTION("Left Trim is correct") {
                        REQUIRE_NOTHROW(partp->ltrim(this_.length()));
                        REQUIRE_THAT(partp->get(), Equals(it_is_what_it_is_buf));
                    }
                }
            }

            SECTION("Can append a byte vector") {

                REQUIRE_NOTHROW(partp->append(mathematics_is_cool));
                REQUIRE(partp->get_size() == mathematics_is_cool.length());

                SECTION("Get is correct") {
                    REQUIRE_THAT(partp->get(), Equals(mathematics_is_cool_buf));
                }

                SECTION("Right Trim is correct") {

                    REQUIRE_NOTHROW(partp->rtrim(_cool.length()));
                    REQUIRE_THAT(partp->get(), Equals(mathematics_is_buf));

                    SECTION("Left Trim is correct") {
                        REQUIRE_NOTHROW(partp->ltrim(mathematics_.length()));
                        REQUIRE_THAT(partp->get(), Equals(it_is_what_it_is_buf));
                    }
                }
            }

            SECTION("Can append integers") {

                REQUIRE_NOTHROW(partp->append(abcd_value));
                REQUIRE_NOTHROW(partp->append(efgh_value));

                SECTION("Get is correct") {
                    REQUIRE_THAT(partp->get(), Equals(abcdefgh_value_buf));
                }

                uint32_t value = 0;

                CHECK(!value);

                SECTION("Left Trim is correct") {
                    REQUIRE_NOTHROW(partp->ltrim(value));
                    REQUIRE(value == abcd_value);
                    REQUIRE_THAT(partp->get(), Equals(efgh_value_buf));
                }

                SECTION("Right Trim is correct") {
                    REQUIRE_NOTHROW(partp->rtrim(value));
                    REQUIRE(value == efgh_value);
                    REQUIRE_THAT(partp->get(), Equals(abcd_value_buf));
                }
            }

            SECTION("Can prepend integers") {

                // Notice in reverse order from the append.
                REQUIRE_NOTHROW(partp->prepend(efgh_value));
                REQUIRE_NOTHROW(partp->prepend(abcd_value));

                SECTION("Get is correct") {
                    REQUIRE_THAT(partp->get(), Equals(abcdefgh_value_buf));
                }

                uint32_t value = 0;

                CHECK(!value);

                SECTION("Left Trim is correct") {
                    REQUIRE_NOTHROW(partp->ltrim(value));
                    REQUIRE(value == abcd_value);
                    REQUIRE_THAT(partp->get(), Equals(efgh_value_buf));
                }

                SECTION("Right Trim is correct") {
                    REQUIRE_NOTHROW(partp->rtrim(value));
                    REQUIRE(value == efgh_value);
                    REQUIRE_THAT(partp->get(), Equals(abcd_value_buf));
                }
            }
        }

        SECTION("Message is properly reset") {
            REQUIRE_NOTHROW(bmp.reset());
        }
    }
}
