//
// Copyright (c) 2017 Michel W Powell <mwpowellhtx@gmail.com>
//
// This software is supplied under the terms of the MIT License, a
// copy of which should be located in the distribution where this
// file was obtained (LICENSE.txt).  A copy of the license may also be
// found online at https://opensource.org/licenses/MIT.
//

// TODO: TBD: unit testing is the right idea, however, these no longer exist apart from binary message

#include <nngcpp.h>

#include "../catch/catch_exception_translations.hpp"

#include "binary_message_part_fixtures.hpp"

namespace constants {
    const nng::messaging::buffer_vector_type data = { 1,2,3 };
}

TEST_CASE("Test that the data is as expected", "[messaging][data]") {

    using namespace nng::messaging;
    using namespace constants;
    using namespace Catch::Matchers;

    buffer_vector_type expected = { 1,2,3 };
    REQUIRE(data.size() == expected.size());
    REQUIRE_THAT(data, Equals(expected));
}

TEST_CASE("Test that C style NNG message appends", "[messaging][header][nng][append]") {

    using namespace nng::messaging;
    using namespace constants;

    ::nng_msg* msgp;

    REQUIRE(::nng_msg_alloc(&msgp, 0) == 0);

    REQUIRE(::nng_msg_header_len(msgp) == 0);

    REQUIRE(data.size() == 3);

    REQUIRE(::nng_msg_header_append(msgp, (const void*)&data[0], data.size()) == 0);

    auto actual_len = ::nng_msg_header_len(msgp);

    REQUIRE(actual_len == data.size());

    auto* p = (buffer_vector_type::value_type*)::nng_msg_header(msgp);

    // Do a poor-man's comparison of each individual element; should align with the expected.
    for (auto i = 0; i < actual_len; i++) {
        const auto expected = *(data.begin() + i);
        const auto actual = (p + i)[0];
        REQUIRE(actual == expected);
    }

    ::nng_msg_free(msgp);
}

TEST_CASE("Test that the default message works", "[messaging][binary][header][default]") {

    using namespace nng::messaging;
    using namespace constants;
    using namespace Catch::Matchers;

    auto part = binary_message_part_fixture<binary_message_header>();

    verify_default_message_part(part);
}

TEST_CASE("::nng_msg* based ctor works", "[messaging][binary][header][alloc]") {

    using namespace trx;
    using namespace nng::messaging;
    using namespace constants;
    using namespace Catch::Matchers;

    ::nng_msg* msgp = nullptr;

    CHECK(msgp == nullptr);

    // In context with the Messaging framework, this would be provided from the binary_message.
    REQUIRE(::nng_msg_alloc(&msgp, 0) == 0);

    REQUIRE(msgp != nullptr);

    // For test purposes, the fixture will assume ownership of the pointer, which will clean up on destruction.
    auto part = binary_message_part_fixture<binary_message_header>(msgp);

    verify_default_message_part(part, msgp);

    // TODO: TBD: may want to further verify the exception what message is correct...
    REQUIRE_THROWS_AS(part.append(data), not_implemented);

    REQUIRE_THROWS_AS(part.insert(data), not_implemented);

    REQUIRE_THROWS_AS(part.chop(), not_implemented);

    REQUIRE_THROWS_AS(part.trim(), not_implemented);
}
