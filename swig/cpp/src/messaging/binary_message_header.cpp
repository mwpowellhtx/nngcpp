//
// Copyright (c) 2017 Michel W Powell <mwpowellhtx@gmail.com>
//
// This software is supplied under the terms of the MIT License, a
// copy of which should be located in the distribution where this
// file was obtained (LICENSE.txt).  A copy of the license may also be
// found online at https://opensource.org/licenses/MIT.
//

#include "binary_message_header.h"

namespace nng {

    namespace messaging {

        using std::placeholders::_1;
        using std::placeholders::_2;
        using std::placeholders::_3;

        binary_message_header::binary_message_header() : readonly_messaging_api() {
        }

        binary_message_header::binary_message_header(::nng_msg* msgp) : readonly_messaging_api(msgp) {
        }

        binary_message_header::~binary_message_header() {
        }

        ::nng_msg* binary_message_header::get_msgp() const {
            return message_base::get_msgp();
        }

        message_base::size_type binary_message_header::get_size() const {
            return _msgp == nullptr ? 0 : ::nng_msg_header_len(_msgp);
        }

        bool binary_message_header::try_get(buffer_vector_type& value) const {

            const auto sz = get_size();

            const auto op = std::bind(&::nng_msg_header, _1);

            return messaging_api_type::try_get(value, op, sz);
        }

        void binary_message_header::clear() {
            const auto op = std::bind(&::nng_msg_header_clear, _1);
            do_clear_op(op, _msgp);
        }

        bool binary_message_header::has_message() const {
            // Which re-exposes has_message for public consumption.
            return message_base::has_message();
        }

        //// TODO: TBD: for the moment, these are overridden at the read-only API level with empty implementations.
        //void binary_message_header::append(const message_base::buffer_vector_type& buf) {
        //    static const auto op = std::bind(&::nng_msg_header_append, _1, _2, _3);
        //    do_type_based_op(op, _msgp, buf);
        //}

        //void binary_message_header::insert(const message_base::buffer_vector_type& buf) {
        //    static const auto op = std::bind(&::nng_msg_header_insert, _1, _2, _3);
        //    do_type_based_op(op, _msgp, buf);
        //}

        //void binary_message_header::trim(size_type sz) {
        //    static const auto op = std::bind(&::nng_msg_header_trim, _1, _2);
        //    do_size_based_op(op, _msgp, sz);
        //}

        //void binary_message_header::chop(size_type sz) {
        //    static const auto op = std::bind(&::nng_msg_header_chop, _1, _2);
        //    do_size_based_op(op, _msgp, sz);
        //}
    }
}
