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

    _HeaderMessagePart::_HeaderMessagePart(_MessageBase* const basep)
        : _MessagePart(basep) {
    }

    _HeaderMessagePart::~_HeaderMessagePart() {
    }

    bool _HeaderMessagePart::HasOne() const {
        //return _MessagePart::HasOne();
        return get_message() != nullptr;
    }

    size_type _HeaderMessagePart::GetSize() {
        const auto msgp = get_message();
        const auto op = bind(&::nng_msg_header_len, msgp);
        return msgp == nullptr ? 0 : op();
    }

    const buffer_vector_type _HeaderMessagePart::Get() {
        return ICanGetType::Get();
    }

    // TODO: TBD: this is fairly redundant with body: there has got to be a better way to capture this as a cross cutting concern...
    bool _HeaderMessagePart::TryGet(buffer_vector_type const* resultp) {

        if (!HasOne()) { return false; }

        typedef message_getter_try_get_policy<buffer_vector_type*, void*> policy_type;
        using PolTy = policy_type;

        const auto sz = GetSize();
        const auto get_ = bind(&::nng_msg_header, _1);

        const auto convert_ = [sz](PolTy::intermediate_type xp, PolTy::result_type yp) {
            if (xp == nullptr) { return false; }
            auto* const src = (const buffer_vector_type::value_type*)xp;
            for (size_type i = 0; i < sz; i++) {
                yp->push_back((src + i)[0]);
            }
            return yp->size() > 0;
        };

        return policy_type::TryGet(const_cast<PolTy::result_type>(resultp), get_message(), get_, convert_);
    }

    void _HeaderMessagePart::Clear() {
        if (!HasOne()) { return; }
        const auto op = bind(&::nng_msg_header_clear, get_message());
        invocation::with_void_return_value(op);
    }

    void _HeaderMessagePart::Append(uint32_t val) {
        if (!HasOne()) { return; }
        const auto op = bind(&::nng_msg_header_append_u32, get_message(), _1);
        invocation::with_default_error_handling(op, val);
    }

    void _HeaderMessagePart::Append(const buffer_vector_type& buf) {
        THROW_API_IS_READ_ONLY();
    }

    void _HeaderMessagePart::Append(const std::string& s) {
        THROW_API_IS_READ_ONLY();
    }

    void _HeaderMessagePart::Prepend(uint32_t val) {
        if (!HasOne()) { return; }
        const auto op = bind(&::nng_msg_header_insert_u32, get_message(), _1);
        invocation::with_default_error_handling(op, val);
    }

    void _HeaderMessagePart::Prepend(const buffer_vector_type& buf) {
        THROW_API_IS_READ_ONLY();
    }

    void _HeaderMessagePart::Prepend(const std::string& s) {
        THROW_API_IS_READ_ONLY();
    }

    void _HeaderMessagePart::TrimLeft(uint32_t* resultp) {
        if (!HasOne()) { return; }
        const auto op = bind(&::nng_msg_header_trim_u32, get_message(), _1);
        invocation::with_default_error_handling(op, resultp);
    }

    void _HeaderMessagePart::TrimLeft(size_type sz) {
        THROW_API_IS_READ_ONLY();
    }

    void _HeaderMessagePart::TrimRight(uint32_t* resultp) {
        if (!HasOne()) { return; }
        const auto op = bind(&::nng_msg_header_chop_u32, get_message(), _1);
        invocation::with_default_error_handling(op, resultp);
    }

    void _HeaderMessagePart::TrimRight(size_type sz) {
        THROW_API_IS_READ_ONLY();
    }

    //void _HeaderMessagePart::Append(const message_base::buffer_vector_type& buf) {
    //    static const auto op = std::bind(&::nng_msg_header_append, _1, _2, _3);
    //    do_type_based_op(op, _msgp, buf);
    //}

    //void _HeaderMessagePart::Prepend(const message_base::buffer_vector_type& buf) {
    //    static const auto op = std::bind(&::nng_msg_header_insert, _1, _2, _3);
    //    do_type_based_op(op, _msgp, buf);
    //}

    //void _HeaderMessagePart::TrimRight(size_type sz) {
    //    static const auto op = std::bind(&::nng_msg_header_chop, _1, _2);
    //    do_size_based_op(op, _msgp, sz);
    //}

    //void _HeaderMessagePart::TrimLeft(size_type sz) {
    //    static const auto op = std::bind(&::nng_msg_header_trim, _1, _2);
    //    do_size_based_op(op, _msgp, sz);
    //}
}
