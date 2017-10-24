//
// Copyright (c) 2017 Michael W Powell <mwpowellhtx@gmail.com>
//
// This software is supplied under the terms of the MIT License, a
// copy of which should be located in the distribution where this
// file was obtained (LICENSE.txt).  A copy of the license may also be
// found online at https://opensource.org/licenses/MIT.
//

#include <nngcpp.h>

#include "../catch/catch_tags.h"

#include "../helpers/constants.h"

namespace constants {
    const std::string hello = "hello";
    const auto hello_buf = to_buffer(hello);
}

TEST_CASE("Run some gymnastic conversion scenarios", Catch::Tags(
    "convert", "messaging", "gymnastics", "cxx").c_str()) {

    using namespace std;
    using namespace nng;
    using namespace Catch::Matchers;
    using namespace constants;

    SECTION("Convert from vector to string") {

        const auto converted_ = gymanstic_convert<buffer_vector_type, string, string::value_type>(hello_buf);
        REQUIRE_THAT(converted_, Equals(hello));
    }

    SECTION("Convert from string tp vector") {

        const auto converted_ = gymanstic_convert<string, buffer_vector_type, buffer_vector_type::value_type>(hello);
        REQUIRE_THAT(converted_, Equals(hello_buf));
    }
}

/* If we want more in depth surface testing, look at binary_message tests. Short
of a full on verification, howevre, we do require that a message is allocated. */
#define NNGCPP_TESTS_INITIALIZE_BINARY_MESSAGE(m) \
    nng::binary_message m

TEST_CASE("Can write (append) to and read (get) from binary message using byte vector"
    , Catch::Tags("vector", "write", "append", "read", "get", "binary", "message"
        , "messaging", "messages", "gymnastics", "cxx").c_str()) {

    using namespace std;
    using namespace nng;
    using namespace Catch::Matchers;
    using namespace constants;

    NNGCPP_TESTS_INITIALIZE_BINARY_MESSAGE(bm);

    SECTION("Can write (append) to") {

        REQUIRE_NOTHROW(bm << hello_buf);

        // Do a quick smoke test.
        REQUIRE(bm.get_size() == hello_buf.size());

        SECTION("Can read (get) from") {

            buffer_vector_type read_;
            REQUIRE_NOTHROW(bm >> read_);

            // And do a simple comparison of the result.
            REQUIRE_THAT(read_, Equals(hello_buf));
        }
    }
}

TEST_CASE("Can write (append) to and read (get) from binary message using string"
    , Catch::Tags("string", "write", "append", "read", "get", "binary", "message"
        , "messaging", "messages", "gymnastics", "cxx").c_str()) {

    using namespace std;
    using namespace nng;
    using namespace Catch::Matchers;
    using namespace constants;

    NNGCPP_TESTS_INITIALIZE_BINARY_MESSAGE(bm);

    SECTION("Can write (append) to") {

        REQUIRE_NOTHROW(bm << hello);

        // Do a quick smoke test.
        REQUIRE(bm.get_size() == hello.length());

        SECTION("Can read (get) from") {

            string read_;
            REQUIRE_NOTHROW(bm >> read_);

            // And do a simple comparison of the result.
            REQUIRE_THAT(read_, Equals(hello));
        }
    }
}

// I dislike declarations via MACRO, but in this case I will make an exception.
#define NNGCPP_TESTS_EXPOSE_BINARY_MESSAGE_BODY(bm, bmb) \
    auto* bmb = bm.body(); \
    REQUIRE(bmb != nullptr)

TEST_CASE("Can write (append) to and read (get) from binary message body using byte vector"
    , Catch::Tags("vector", "write", "append", "read", "get", "binary", "message", "body"
        , "messaging", "messages", "gymnastics", "cxx").c_str()) {

    using namespace std;
    using namespace nng;
    using namespace Catch::Matchers;
    using namespace constants;

    // TODO: TBD: should revisit the utility of these macros; with the messaging work I've done, their value has decreased I think...
    NNGCPP_TESTS_INITIALIZE_BINARY_MESSAGE(bm);

    // map - Message API pointer.
    NNGCPP_TESTS_EXPOSE_BINARY_MESSAGE_BODY(bm, mapip);

    SECTION("Can write (append) to") {

        REQUIRE_NOTHROW(bm << hello_buf);

        // Do a quick smoke test.
        REQUIRE(mapip->get_size() == hello_buf.size());

        SECTION("Can read (get) from") {

            buffer_vector_type read_;
            REQUIRE_NOTHROW(*mapip >> read_);

            // And do a simple comparison of the result.
            REQUIRE_THAT(read_, Equals(hello_buf));
        }
    }
}

TEST_CASE("Can write (append) to and read (get) from binary message body using string"
    , Catch::Tags("string", "write", "append", "read", "get", "binary", "message"
        , "body", "messaging", "messages", "gymnastics", "cxx").c_str()) {

    using namespace std;
    using namespace nng;
    using namespace Catch::Matchers;
    using namespace constants;

    NNGCPP_TESTS_INITIALIZE_BINARY_MESSAGE(bm);

    NNGCPP_TESTS_EXPOSE_BINARY_MESSAGE_BODY(bm, mapip);

    SECTION("Can write (append) to") {

        REQUIRE_NOTHROW(bm << hello);

        // Do a quick smoke test.
        REQUIRE(mapip->get_size() == hello.length());

        SECTION("Can read (get) from") {

            string read_;
            REQUIRE_NOTHROW(*mapip >> read_);

            // And do a simple comparison of the result.
            REQUIRE_THAT(read_, Equals(hello));
        }
    }
}

/* I was going to put test cases in for the header, but I think for the moment will hold off
on that due to the fact that we really should not be mucking around in the header. */
