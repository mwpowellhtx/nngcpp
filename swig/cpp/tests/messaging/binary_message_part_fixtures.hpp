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

#include "messaging/message_base.h"

namespace nng {
    namespace messaging {

        template<class Base_>
        struct binary_message_part_fixture : public Base_ {

            typedef Base_ base_type;

            typedef message_base::buffer_vector_type buffer_vector_type;

            typedef message_base::size_type size_type;

            binary_message_part_fixture()
                : base_type() {
            }

            binary_message_part_fixture(::nng_msg* msgp)
                : base_type(msgp) {
            }

            virtual ~binary_message_part_fixture() {

                if (_msgp == nullptr) { return; }

                REQUIRE(_msgp != nullptr);
                CHECK_NOTHROW(::nng_msg_free(_msgp));

                _msgp = nullptr;
                CHECK(_msgp == nullptr);
            }

            virtual void append(const buffer_vector_type& x) override {
                base_type::append(x);
            }

            virtual void insert(const buffer_vector_type& x) override {
                base_type::insert(x);
            }

            virtual void trim(size_type sz = 0) override {
                base_type::trim(sz);
            }

            virtual void chop(size_type sz = 0) override {
                base_type::chop(sz);
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
