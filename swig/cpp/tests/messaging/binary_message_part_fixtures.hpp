//
// Copyright (c) 2017 Michel W Powell <mwpowellhtx@gmail.com>
//
// This software is supplied under the terms of the MIT License, a
// copy of which should be located in the distribution where this
// file was obtained (LICENSE.txt).  A copy of the license may also be
// found online at https://opensource.org/licenses/MIT.
//

#ifndef NNGCPP_TESTS_MESSAGING_BINARY_MESSAGE_PART_FIXTURES_HPP
#define NNGCPP_TESTS_MESSAGING_BINARY_MESSAGE_PART_FIXTURES_HPP

#include <catch.hpp>
#include <nng/nng.h>

namespace nng {
    namespace messaging {

        template<class Base_>
        struct binary_message_part_fixture : public Base_ {

            binary_message_part_fixture()
                : Base_() {
            }

            binary_message_part_fixture(::nng_msg* msgp)
                : Base_(msgp) {
            }

            virtual ~binary_message_part_fixture() {

                if (_msgp == nullptr) { return; }

                REQUIRE(_msgp != nullptr);
                CHECK_NOTHROW(::nng_msg_free(_msgp));

                _msgp = nullptr;
                CHECK(_msgp == nullptr);
            }
        };

        template<class Part_>
        void verify_default_message_part(const Part_& part, ::nng_msg* msgp = nullptr) {

            const auto expected_has_message = msgp != nullptr;
            const auto actual_has_message = part.has_message();
            REQUIRE(actual_has_message == expected_has_message);

            const auto actual_size = part.get_size();
            REQUIRE(actual_size == 0);
        }
    }
}

#endif // NNGCPP_TESTS_MESSAGING_BINARY_MESSAGE_PART_FIXTURES_HPP
