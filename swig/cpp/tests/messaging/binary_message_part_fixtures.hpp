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

#include "messaging/message_base.h"

#include <catch.hpp>

namespace nng {

#ifndef NNGCPP_BINARY_MESSAGE_H
    template<class Body_, class Header_> class basic_binary_message;
#endif // NNGCPP_BINARY_MESSAGE_H

    template<class Base_>
    struct binary_message_part_fixture : public Base_ {

        typedef Base_ base_type;

        binary_message_part_fixture(message_base* const basep) : base_type(basep) {
        }

        virtual ~binary_message_part_fixture() {
        }

        virtual void append(const uint32_t& x) override {
            base_type::append(x);
        }

        virtual void append(const buffer_vector_type& x) override {
            base_type::append(x);
        }

        virtual void append(const std::string& x) override {
            base_type::append(x);
        }

        virtual void prepend(const uint32_t& x) override {
            base_type::prepend(x);
        }

        virtual void prepend(const buffer_vector_type& x) override {
            base_type::prepend(x);
        }

        virtual void prepend(const std::string& x) override {
            base_type::prepend(x);
        }

        virtual void ltrim(size_type sz = 0) override {
            base_type::ltrim(sz);
        }

        virtual void ltrim(uint32_t& val) override {
            base_type::ltrim(val);
        }

        virtual void rtrim(size_type sz = 0) override {
            base_type::rtrim(sz);
        }

        virtual void rtrim(uint32_t& val) override {
            base_type::rtrim(val);
        }

    protected:

        template<class Body_, class Header_> friend class basic_binary_message;
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

#endif // NNGCPP_TESTS_MESSAGING_BINARY_MESSAGE_PART_FIXTURES_HPP
