//
// Copyright (c) 2017 Michael W Powell <mwpowellhtx@gmail.com>
//
// This software is supplied under the terms of the MIT License, a
// copy of which should be located in the distribution where this
// file was obtained (LICENSE.txt).  A copy of the license may also be
// found online at https://opensource.org/licenses/MIT.
//

#include "messaging_gymnastics.h"

namespace nng {

    binary_message& operator<<(binary_message& lhs, const buffer_vector_type& rhs) {
        auto ops = message_conversion_appender_policy<buffer_vector_type, binary_message>();
        ops.Append(lhs, rhs);
        return lhs;
    }

    binary_message& operator >> (binary_message& lhs, buffer_vector_type& rhs) {
        auto ops = message_conversion_getter_policy<buffer_vector_type, binary_message>();
        rhs = ops.Get(lhs);
        return lhs;
    }

    binary_message& operator<<(binary_message& lhs, const std::string& rhs) {
        auto ops = message_conversion_appender_policy<std::string, binary_message>();
        ops.Append(lhs, rhs);
        return lhs;
    }

    binary_message& operator >> (binary_message& lhs, std::string& rhs) {
        auto ops = message_conversion_getter_policy<std::string, binary_message>();
        rhs = ops.Get(lhs);
        return lhs;
    }

    ISupportsAppend<buffer_vector_type>& operator<<(ISupportsAppend<buffer_vector_type>& lhs, const buffer_vector_type& rhs) {
        auto ops = message_conversion_appender_policy<buffer_vector_type, ISupportsAppend<buffer_vector_type>>();
        ops.Append(lhs, rhs);
        return lhs;
    }

    ISupportsGet<buffer_vector_type>& operator >> (ISupportsGet<buffer_vector_type>& lhs, buffer_vector_type& rhs) {
        auto ops = message_conversion_getter_policy<buffer_vector_type, ISupportsGet<buffer_vector_type>>();
        rhs = ops.Get(lhs);
        return lhs;
    }

    ISupportsAppend<buffer_vector_type>& operator<<(ISupportsAppend<buffer_vector_type>& lhs, const std::string& rhs) {
        auto ops = message_conversion_appender_policy<std::string, ISupportsAppend<buffer_vector_type>>();
        ops.Append(lhs, rhs);
        return lhs;
    }

    ISupportsGet<buffer_vector_type>& operator >> (ISupportsGet<buffer_vector_type>& lhs, std::string& rhs) {
        auto ops = message_conversion_getter_policy<std::string, ISupportsGet<buffer_vector_type>>();
        rhs = ops.Get(lhs);
        return lhs;
    }
}
