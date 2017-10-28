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

//#include "core/types.h"

namespace nng {

#ifndef NNGCPP_BINARY_MESSAGE_H
    template<class Body_, class Header_> class _BasicMessage;
#endif // NNGCPP_BINARY_MESSAGE_H

    template<class Base_>
    struct binary_message_part_fixture : public Base_ {

        typedef Base_ base_type;

        binary_message_part_fixture(message_base* const basep) : base_type(basep) {
        }

        virtual ~binary_message_part_fixture() {
        }

        virtual void Append(uint32_t x) override {
            base_type::Append(x);
        }

        virtual void Append(const buffer_vector_type& x) override {
            base_type::Append(x);
        }

        virtual void Append(const std::string& x) override {
            base_type::Append(x);
        }

        virtual void Prepend(uint32_t x) override {
            base_type::Prepend(x);
        }

        virtual void Prepend(const buffer_vector_type& x) override {
            base_type::Prepend(x);
        }

        virtual void Prepend(const std::string& x) override {
            base_type::Prepend(x);
        }

        virtual void TrimLeft(size_type sz = 0) override {
            base_type::TrimLeft(sz);
        }

        virtual void TrimLeft(uint32_t* resultp) override {
            base_type::TrimLeft(resultp);
        }

        virtual void TrimRight(size_type sz = 0) override {
            base_type::TrimRight(sz);
        }

        virtual void TrimRight(uint32_t* resultp) override {
            base_type::TrimRight(resultp);
        }

    protected:

        template<class Body_, class Header_> friend class _BasicMessage;
    };

    template<class Part_>
    void verify_default_message_part(const Part_& part, msg_type* msgp = nullptr) {

        const auto expected_has_message = msgp != nullptr;
        const auto actual_has_message = part.has_message();
        REQUIRE(actual_has_message == expected_has_message);

        const auto actual_size = part.GetSize();
        REQUIRE(actual_size == 0);
    }
}

#endif // NNGCPP_TESTS_MESSAGING_BINARY_MESSAGE_PART_FIXTURES_HPP
