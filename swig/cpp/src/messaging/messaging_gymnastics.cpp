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
    namespace messaging {

        binary_message& operator<<(binary_message& lhs, const buffer_vector_type& rhs) {
            auto ops = message_conversion_proxy<buffer_vector_type, binary_message>();
            ops.append(lhs, rhs);
            return lhs;
        }

        const binary_message& operator >> (const binary_message& lhs, buffer_vector_type& rhs) {
            auto ops = message_conversion_proxy<buffer_vector_type, binary_message>();
            rhs = ops.get(lhs);
            return lhs;
        }

        binary_message& operator<<(binary_message& lhs, const std::string& rhs) {
            auto ops = message_conversion_proxy<std::string, binary_message>();
            ops.append(lhs, rhs);
            return lhs;
        }

        const binary_message& operator >> (const binary_message& lhs, std::string& rhs) {
            auto ops = message_conversion_proxy<std::string, binary_message>();
            rhs = ops.get(lhs);
            return lhs;
        }

        messaging_api<buffer_vector_type>& operator<<(messaging_api<buffer_vector_type>& lhs, const buffer_vector_type& rhs) {
            auto ops = message_conversion_proxy<buffer_vector_type, messaging_api<buffer_vector_type>>();
            ops.append(lhs, rhs);
            return lhs;
        }

        const messaging_api<buffer_vector_type>& operator >> (const messaging_api<buffer_vector_type>& lhs, buffer_vector_type& rhs) {
            auto ops = message_conversion_proxy<buffer_vector_type, messaging_api<buffer_vector_type>>();
            rhs = ops.get(lhs);
            return lhs;
        }

        messaging_api<buffer_vector_type>& operator<<(messaging_api<buffer_vector_type>& lhs, const std::string& rhs) {
            auto ops = message_conversion_proxy<std::string, messaging_api<buffer_vector_type>>();
            ops.append(lhs, rhs);
            return lhs;
        }

        const messaging_api<buffer_vector_type>& operator >> (const messaging_api<buffer_vector_type>& lhs, std::string& rhs) {
            auto ops = message_conversion_proxy<std::string, messaging_api<buffer_vector_type>>();
            rhs = ops.get(lhs);
            return lhs;
        }
    }
}
