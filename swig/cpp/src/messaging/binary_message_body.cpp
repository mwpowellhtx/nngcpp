//
// Copyright (c) 2017 Michel W Powell <mwpowellhtx@gmail.com>
//
// This software is supplied under the terms of the MIT License, a
// copy of which should be located in the distribution where this
// file was obtained (LICENSE.txt).  A copy of the license may also be
// found online at https://opensource.org/licenses/MIT.
//

#include "binary_message_body.h"
#include "../core/invocation.hpp"

namespace nng {

    using std::placeholders::_1;
    using std::placeholders::_2;
    using std::placeholders::_3;

    binary_message_body::binary_message_body(message_base* basep) : message_part(basep) {
    }

    binary_message_body::~binary_message_body() {
    }

    size_type binary_message_body::get_size() {
        const auto msgp = get_msgp();
        auto result = static_cast<size_type>(0);
        if (msgp == nullptr) { return result; }
        const auto op = std::bind(&::nng_msg_len, msgp);
        invocation::with_result(op, &result);
        return result;
    }

    bool binary_message_body::try_get(buffer_vector_type& value) {

        const auto msgp = get_msgp();

        if (msgp == nullptr) { return false; }

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

        return policy_type::try_get(value, msgp, get_, convert_);
    }

    void binary_message_body::clear() {
        const auto msgp = get_msgp();
        if (msgp == nullptr) { return; }
        const auto op = std::bind(&::nng_msg_clear, msgp);
        invocation::with_void_return_value(op);
    }

    void binary_message_body::append(const buffer_vector_type& buf) {
        const auto msgp = get_msgp();
        // Also save calories if there are no bytes to append.
        if (msgp == nullptr || !buf.size()) { return; }
        // Bind to concrete message ptr along same lines as with options API.
        const auto op = std::bind(&::nng_msg_append, msgp, _1, _2);
        invocation::with_default_error_handling(op, (const void*)&buf[0], buf.size());
    }

    void binary_message_body::prepend(const buffer_vector_type& buf) {
        const auto msgp = get_msgp();
        // Also save calories if there are no bytes to prepend.
        if (msgp == nullptr || !buf.size()) { return; }
        const auto op = std::bind(&::nng_msg_insert, msgp, _1, _2);
        invocation::with_default_error_handling(op, (const void*)&buf[0], buf.size());
    }

    void binary_message_body::ltrim(size_type sz) {
        const auto msgp = get_msgp();
        if (msgp == nullptr) { return; }
        const auto op = std::bind(&::nng_msg_trim, msgp, _1);
        invocation::with_default_error_handling(op, sz);
    }

    void binary_message_body::rtrim(size_type sz) {
        const auto msgp = get_msgp();
        if (msgp == nullptr) { return; }
        const auto op = std::bind(&::nng_msg_chop, msgp, _1);
        invocation::with_default_error_handling(op, sz);
    }

    void binary_message_body::append(const uint32_t& val) {
        const auto msgp = get_msgp();
        if (msgp == nullptr) { return; }
        const auto op = std::bind(&::nng_msg_append_u32, msgp, _1);
        invocation::with_default_error_handling(op, val);
    }

    void binary_message_body::prepend(const uint32_t& val) {
        const auto msgp = get_msgp();
        if (msgp == nullptr) { return; }
        const auto op = std::bind(&::nng_msg_insert_u32, msgp, _1);
        invocation::with_default_error_handling(op, val);
    }

    void binary_message_body::ltrim(uint32_t& val) {
        const auto msgp = get_msgp();
        if (msgp == nullptr) { return; }
        const auto op = std::bind(&::nng_msg_trim_u32, msgp, _1);
        invocation::with_default_error_handling(op, &val);
    }

    void binary_message_body::rtrim(uint32_t& val) {
        const auto msgp = get_msgp();
        if (msgp == nullptr) { return; }
        const auto op = std::bind(&::nng_msg_chop_u32, msgp, _1);
        invocation::with_default_error_handling(op, &val);
    }

    void binary_message_body::append(const std::string& s) {
        // Save the calories when we do not have an internal message.
        const auto msgp = get_msgp();
        // Also bypass when the string is empty.
        if (msgp == nullptr || !s.length()) { return; }
        buffer_vector_type buf(s.cbegin(), s.cend());
        // Which is a little redundant, but it saves calories.
        append(buf);
    }

    void binary_message_body::prepend(const std::string& s) {
        // Save the calories when we do not have an internal message.
        const auto msgp = get_msgp();
        // Also bypass when the string is empty.
        if (msgp == nullptr || !s.length()) { return; }
        buffer_vector_type buf(s.cbegin(), s.cend());
        // Which is a little redundant, but it saves calories.
        prepend(buf);
    }
}
