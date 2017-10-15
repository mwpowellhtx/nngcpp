//
// Copyright (c) 2017 Michel W Powell <mwpowellhtx@gmail.com>
//
// This software is supplied under the terms of the MIT License, a
// copy of which should be located in the distribution where this
// file was obtained (LICENSE.txt).  A copy of the license may also be
// found online at https://opensource.org/licenses/MIT.
//

#include "binary_message_body.h"
#include "../core/exceptions.hpp"

namespace nng {

    namespace messaging {

        using std::placeholders::_1;
        using std::placeholders::_2;
        using std::placeholders::_3;

        binary_message_body::binary_message_body() : message_base() {
        }

        binary_message_body::binary_message_body(::nng_msg* msgp) : message_base(msgp) {
        }

        binary_message_body::~binary_message_body() {
        }

        binary_message_body::size_type binary_message_body::get_size() const {
            const auto op = std::bind(&::nng_msg_len, _1);
            return _msgp == nullptr ? 0 : op(_msgp);
        }

        bool binary_message_body::try_get(buffer_vector_type& value) const {

            if (_msgp == nullptr) { return false; }

            typedef message_getter_try_get_policy<buffer_vector_type, void*> policy_type;

            const auto sz = get_size();

            const auto get_ = std::bind(&::nng_msg_body, _1);

            const auto convert_ = [&sz](void* x, buffer_vector_type& y) {
                if (x == nullptr) { return false; }
                auto* src = (const buffer_vector_type::value_type*)x;
                for (size_type i = 0; i < sz; i++) {
                    y.push_back((src + i)[0]);
                }
                return y.size() > 0;
            };

            return policy_type::try_get(value, _msgp, get_, convert_);
        }

        void binary_message_body::clear() {
            if (_msgp == nullptr) { return; }
            const auto op = std::bind(&::nng_msg_clear, _1);
            do_clear_op(op, _msgp);
        }

        void binary_message_body::append(const buffer_vector_type& buf) {
            if (_msgp == nullptr) { return; }
            const auto op = std::bind(&::nng_msg_append, _1, _2, _3);
            const auto errnum = op(_msgp, (const void*)&buf[0], buf.size());
            THROW_NNG_EXCEPTION_EC(errnum);
        }

        void binary_message_body::insert(const buffer_vector_type& buf) {
            if (_msgp == nullptr) { return; }
            const auto op = std::bind(&::nng_msg_insert, _1, _2, _3);
            const auto errnum = op(_msgp, (const void*)&buf[0], buf.size());
            THROW_NNG_EXCEPTION_EC(errnum);
        }

        void binary_message_body::trim(size_type sz) {
            if (_msgp == nullptr) { return; }
            const auto op = std::bind(&::nng_msg_trim, _1, _2);
            const auto errnum = op(_msgp, sz);
            THROW_NNG_EXCEPTION_EC(errnum);
        }

        void binary_message_body::chop(size_type sz) {
            if (_msgp == nullptr) { return; }
            const auto op = std::bind(&::nng_msg_chop, _1, _2);
            const auto errnum = op(_msgp, sz);
            THROW_NNG_EXCEPTION_EC(errnum);
        }

        void binary_message_body::append(const uint32_t& val) {
            if (_msgp == nullptr) { return; }
            const auto op = std::bind(&::nng_msg_append_u32, _1, _2);
            const auto errnum = op(_msgp, val);
            THROW_NNG_EXCEPTION_EC(errnum);
        }

        void binary_message_body::insert(const uint32_t& val) {
            if (_msgp == nullptr) { return; }
            const auto op = std::bind(&::nng_msg_insert_u32, _1, _2);
            const auto errnum = op(_msgp, val);
            THROW_NNG_EXCEPTION_EC(errnum);
        }

        void binary_message_body::trim(uint32_t* valp) {
            if (_msgp == nullptr) { return; }
            const auto op = std::bind(&::nng_msg_trim_u32, _1, _2);
            const auto errnum = op(_msgp, valp);
            THROW_NNG_EXCEPTION_EC(errnum);
        }

        void binary_message_body::chop(uint32_t* valp) {
            if (_msgp == nullptr) { return; }
            const auto op = std::bind(&::nng_msg_chop_u32, _1, _2);
            const auto errnum = op(_msgp, valp);
            THROW_NNG_EXCEPTION_EC(errnum);
        }
    }
}
