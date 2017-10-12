//
// Copyright (c) 2017 Michel W Powell <mwpowellhtx@gmail.com>
//
// This software is supplied under the terms of the MIT License, a
// copy of which should be located in the distribution where this
// file was obtained (LICENSE.txt).  A copy of the license may also be
// found online at https://opensource.org/licenses/MIT.
//

#include "binary_message_body.h"

namespace nng {

    namespace messaging {

        using std::placeholders::_1;
        using std::placeholders::_2;
        using std::placeholders::_3;

        binary_message_body::binary_message_body() : messaging_api() {
        }

        binary_message_body::binary_message_body(::nng_msg* msgp) : messaging_api(msgp) {
        }

        binary_message_body::~binary_message_body() {
        }

        binary_message_body::size_type binary_message_body::get_size() const {
            return _msgp == nullptr ? 0 : ::nng_msg_len(_msgp);
        }

        const binary_message_body::buffer_vector_type binary_message_body::get() const {

            buffer_vector_type buf;

            return try_get(buf) ? buf : buffer_vector_type();
        }

        bool binary_message_body::try_get(buffer_vector_type& value) const {

            const auto sz = get_size();

            const auto op = std::bind(&::nng_msg_body, _1);

            return messaging_api_type::try_get(value, op, sz);
        }

        void binary_message_body::clear() {
            ::nng_msg_clear(_msgp);
        }

        void binary_message_body::append(const buffer_vector_type& buf) {
            static const auto op = std::bind(&::nng_msg_append, _1, _2, _3);
            do_type_based_op(op, _msgp, buf);
        }

        void binary_message_body::insert(const buffer_vector_type& buf) {
            if (_msgp == nullptr) { return; }
            static const auto op = std::bind(&::nng_msg_insert, _1, _2, _3);
            do_type_based_op(op, _msgp, buf);
        }

        void binary_message_body::trim(size_type sz) {
            if (_msgp == nullptr) { return; }
            static const auto op = std::bind(&::nng_msg_trim, _1, _2);
            do_size_based_op(op, _msgp, sz);
        }

        void binary_message_body::chop(size_type sz) {
            if (_msgp == nullptr) { return; }
            static const auto op = std::bind(&::nng_msg_chop, _1, _2);
            do_size_based_op(op, _msgp, sz);
        }
    }
}
