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

    _BodyMessagePart::_BodyMessagePart(_MessageBase* basep)
        : _MessagePart(basep) {
    }

    _BodyMessagePart::~_BodyMessagePart() {
    }

    bool _BodyMessagePart::HasOne() const {
        //return _MessagePart::HasOne();
        return get_message() != nullptr;
    }

    size_type _BodyMessagePart::GetSize() {
        auto result = static_cast<size_type>(0);
        if (!HasOne()) { return result; }
        const auto op = std::bind(&::nng_msg_len, get_message());
        invocation::with_result(op, &result);
        return result;
    }

    const buffer_vector_type _BodyMessagePart::Get() {
        return ICanGetType::Get();
    }

    bool _BodyMessagePart::TryGet(buffer_vector_type const* resultp) {

        if (!HasOne()) { return false; }

        typedef message_getter_try_get_policy<buffer_vector_type*, void*> policy_type;
        using PolTy = policy_type;

        const auto sz = GetSize();
        const auto get_ = std::bind(&::nng_msg_body, _1);

        const auto convert_ = [sz](PolTy::intermediate_type xp, PolTy::result_type yp) {
            if (xp == nullptr) { return false; }
            auto* src = (const buffer_vector_type::value_type*)xp;
            for (size_type i = 0; i < sz; i++) {
                yp->push_back((src + i)[0]);
            }
            return yp->size() > 0;
        };

        return policy_type::TryGet(const_cast<PolTy::result_type>(resultp), get_message(), get_, convert_);
    }

    void _BodyMessagePart::Clear() {
        if (!HasOne()) { return; }
        const auto op = std::bind(&::nng_msg_clear, get_message());
        invocation::with_void_return_value(op);
    }

    void _BodyMessagePart::Append(const buffer_vector_type& buf) {
        // Also save calories if there are no bytes to append.
        if (!(HasOne() || buf.size())) { return; }
        // Bind to concrete message ptr along same lines as with options API.
        const auto op = std::bind(&::nng_msg_append, get_message(), _1, _2);
        invocation::with_default_error_handling(op, (const void*)&buf[0], buf.size());
    }

    void _BodyMessagePart::Append(const std::string& s) {
        // Also bypass when the string is empty.
        if (!(HasOne() || s.length())) { return; }
        buffer_vector_type buf(s.cbegin(), s.cend());
        // Which is a little redundant, but it saves calories.
        Append(buf);
    }

    void _BodyMessagePart::Append(uint32_t val) {
        if (!HasOne()) { return; }
        const auto op = std::bind(&::nng_msg_append_u32, get_message(), _1);
        invocation::with_default_error_handling(op, val);
    }

    void _BodyMessagePart::Prepend(const buffer_vector_type& buf) {
        // Also save calories if there are no bytes to prepend.
        if (!(HasOne() || buf.size())) { return; }
        const auto op = std::bind(&::nng_msg_insert, get_message(), _1, _2);
        invocation::with_default_error_handling(op, (const void*)&buf[0], buf.size());
    }

    void _BodyMessagePart::Prepend(const std::string& s) {
        // Also bypass when the string is empty.
        if (!(HasOne() || s.length())) { return; }
        buffer_vector_type buf(s.cbegin(), s.cend());
        // Which is a little redundant, but it saves calories.
        Prepend(buf);
    }

    void _BodyMessagePart::Prepend(uint32_t val) {
        if (!HasOne()) { return; }
        const auto op = std::bind(&::nng_msg_insert_u32, get_message(), _1);
        invocation::with_default_error_handling(op, val);
    }

    void _BodyMessagePart::TrimLeft(size_type sz) {
        if (!HasOne()) { return; }
        const auto op = std::bind(&::nng_msg_trim, get_message(), _1);
        invocation::with_default_error_handling(op, sz);
    }

    void _BodyMessagePart::TrimLeft(uint32_t* resultp) {
        if (!HasOne()) { return; }
        const auto op = std::bind(&::nng_msg_trim_u32, get_message(), _1);
        invocation::with_default_error_handling(op, resultp);
    }

    void _BodyMessagePart::TrimRight(size_type sz) {
        if (!HasOne()) { return; }
        const auto op = std::bind(&::nng_msg_chop, get_message(), _1);
        invocation::with_default_error_handling(op, sz);
    }

    void _BodyMessagePart::TrimRight(uint32_t* resultp) {
        if (!HasOne()) { return; }
        const auto op = std::bind(&::nng_msg_chop_u32, get_message(), _1);
        invocation::with_default_error_handling(op, resultp);
    }
}
