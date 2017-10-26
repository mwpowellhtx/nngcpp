//
// Copyright (c) 2017 Michel W Powell <mwpowellhtx@gmail.com>
//
// This software is supplied under the terms of the MIT License, a
// copy of which should be located in the distribution where this
// file was obtained (LICENSE.txt).  A copy of the license may also be
// found online at https://opensource.org/licenses/MIT.
//

#include "binary_message_header.h"
#include "../core/invocation.hpp"

#ifndef THROW_API_IS_READ_ONLY
#define THROW_API_IS_READ_ONLY() throw nng::exceptions::not_implemented("API is read-only")
#endif // THROW_API_IS_READ_ONLY

#include <functional>

namespace nng {

    using std::placeholders::_1;
    using std::placeholders::_2;
    using std::bind;

    binary_message_header::binary_message_header(_MessageBase* const basep)
        : message_part(basep) {
    }

    binary_message_header::~binary_message_header() {
    }

    size_type binary_message_header::get_size() {
        const auto msgp = get_msgp();
        const auto op = bind(&::nng_msg_header_len, msgp);
        return msgp == nullptr ? 0 : op();
    }

    // TODO: TBD: this is fairly redundant with body: there has got to be a better way to capture this as a cross cutting concern...
    bool binary_message_header::try_get(buffer_vector_type& value) {

        const auto msgp = get_msgp();

        if (msgp == nullptr) { return false; }

        typedef message_getter_try_get_policy<buffer_vector_type, void*> policy_type;

        const auto get_ = bind(&::nng_msg_header, _1);

        const auto convert_ = [&](const void* x, buffer_vector_type& y) {
            if (x == nullptr) { return false; }
            const auto sz = get_size();
            const auto* const src = (const buffer_vector_type::value_type* const)x;
            for (size_type i = 0; i < sz; i++) {
                y.push_back((src + i)[0]);
            }
            return y.size() > 0;
        };

        return policy_type::try_get(value, msgp, get_, convert_);
    }

    void binary_message_header::Clear() {
        const auto msgp = get_msgp();
        if (!msgp) { return; }
        const auto op = bind(&::nng_msg_header_clear, msgp);
        invocation::with_void_return_value(op);
    }

    void binary_message_header::append(const uint32_t& val) {
        const auto msgp = get_msgp();
        if (msgp == nullptr) { return; }
        const auto op = bind(&::nng_msg_header_append_u32, msgp, _1);
        invocation::with_default_error_handling(op, val);
    }

    void binary_message_header::prepend(const uint32_t& val) {
        const auto msgp = get_msgp();
        if (msgp == nullptr) { return; }
        const auto op = bind(&::nng_msg_header_insert_u32, msgp, _1);
        invocation::with_default_error_handling(op, val);
    }

    void binary_message_header::ltrim(uint32_t& val) {
        const auto msgp = get_msgp();
        if (msgp == nullptr) { return; }
        const auto op = bind(&::nng_msg_header_trim_u32, msgp, _1);
        invocation::with_default_error_handling(op, &val);
    }

    void binary_message_header::rtrim(uint32_t& val) {
        const auto msgp = get_msgp();
        if (msgp == nullptr) { return; }
        const auto op = bind(&::nng_msg_header_chop_u32, msgp, _1);
        invocation::with_default_error_handling(op, &val);
    }

    void binary_message_header::append(const buffer_vector_type& buf) {
        THROW_API_IS_READ_ONLY();
    }

    void binary_message_header::prepend(const buffer_vector_type& buf) {
        THROW_API_IS_READ_ONLY();
    }

    void binary_message_header::rtrim(size_type sz) {
        THROW_API_IS_READ_ONLY();
    }

    void binary_message_header::ltrim(size_type sz) {
        THROW_API_IS_READ_ONLY();
    }

    void binary_message_header::append(const std::string& s) {
        THROW_API_IS_READ_ONLY();
    }

    void binary_message_header::prepend(const std::string& s) {
        THROW_API_IS_READ_ONLY();
    }

    //void binary_message_header::append(const message_base::buffer_vector_type& buf) {
    //    static const auto op = std::bind(&::nng_msg_header_append, _1, _2, _3);
    //    do_type_based_op(op, _msgp, buf);
    //}

    //void binary_message_header::prepend(const message_base::buffer_vector_type& buf) {
    //    static const auto op = std::bind(&::nng_msg_header_insert, _1, _2, _3);
    //    do_type_based_op(op, _msgp, buf);
    //}

    //void binary_message_header::rtrim(size_type sz) {
    //    static const auto op = std::bind(&::nng_msg_header_chop, _1, _2);
    //    do_size_based_op(op, _msgp, sz);
    //}

    //void binary_message_header::ltrim(size_type sz) {
    //    static const auto op = std::bind(&::nng_msg_header_trim, _1, _2);
    //    do_size_based_op(op, _msgp, sz);
    //}
}
