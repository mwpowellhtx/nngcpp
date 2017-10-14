//
// Copyright (c) 2017 Michael W Powell <mwpowellhtx@gmail.com>
//
// This software is supplied under the terms of the MIT License, a
// copy of which should be located in the distribution where this
// file was obtained (LICENSE.txt).  A copy of the license may also be
// found online at https://opensource.org/licenses/MIT.
//

#ifndef NNGCPP_MESSAGING_GYMNASTICS_HPP
#define NNGCPP_MESSAGING_GYMNASTICS_HPP

// TODO: TBD: start here, but the intent is for this to support anything that looks or operates like the writable message api and/or readonly api
#include "binary_message.h"

#include <string>
#include <algorithm>

namespace nng {
    namespace messaging {

        typedef message_base::buffer_vector_type buffer_vector_type;

        template<class Target_, class Message_>
        struct message_conversion_getter_policy {

            typedef Target_ target_type;
            typedef Message_ message_type;
            typedef message_conversion_getter_policy<Target_, Message_> base_type;

            virtual target_type get(const message_type& rhs) {
                return rhs.get();
            }
        };

        template<class Target_, class Message_>
        struct message_conversion_appender_policy {

            typedef Target_ target_type;
            typedef Message_ message_type;
            typedef message_conversion_appender_policy<Target_, Message_> base_type;

            virtual void append(message_type& lhs, const target_type& rhs) {
                lhs.append(rhs);
            }
        };

        template<>
        struct message_conversion_getter_policy<buffer_vector_type, messaging_api<buffer_vector_type>> {

            virtual buffer_vector_type get(const messaging_api<buffer_vector_type>& rhs) {
                return rhs.get();
            }
        };

        template<>
        struct message_conversion_appender_policy<buffer_vector_type, messaging_api<buffer_vector_type>> {

            virtual void append(messaging_api<buffer_vector_type>& lhs, const buffer_vector_type& rhs) {
                lhs.append(rhs);
            }
        };

        template<class Src_, class Dest_, class Value_>
        Dest_ gymanstic_convert(const Src_& src) {
            Dest_ dest;
            for (Src_::const_iterator it = src.cbegin(); it != src.cend(); ++it) {
                dest.push_back((Src_::value_type)*it);
            }
            //const auto push_back = [&](const Value_& x) { dest.push_back(x); };
            //std::for_each(src.cbegin(), src.cend(), push_back);
            return dest;
        }

        template<>
        struct message_conversion_getter_policy<std::string, messaging_api<buffer_vector_type>> {

            virtual std::string get(const messaging_api<buffer_vector_type>& rhs) {
                auto lhs_ = rhs.get();
                return gymanstic_convert<buffer_vector_type, std::string, std::string::value_type>(lhs_);
            }
        };

        template<>
        struct message_conversion_appender_policy<std::string, messaging_api<buffer_vector_type>> {

            virtual void append(messaging_api<buffer_vector_type>& lhs, const std::string& rhs) {
                auto buf_ = gymanstic_convert<std::string, buffer_vector_type, buffer_vector_type::value_type>(rhs);
                lhs.append(buf_);
            }
        };

        template<>
        struct message_conversion_getter_policy<std::string, binary_message> {

            virtual std::string get(const binary_message& rhs) {
                const auto lhs_ = const_cast<binary_message&>(rhs).body()->get();
                return gymanstic_convert<buffer_vector_type, std::string, std::string::value_type>(lhs_);
            }
        };

        template<>
        struct message_conversion_appender_policy<std::string, binary_message> {

            virtual void append(binary_message& lhs, const std::string& rhs) {
                const auto buf_ = gymanstic_convert<std::string, buffer_vector_type, buffer_vector_type::value_type>(rhs);
                lhs.body()->append(buf_);
            }
        };

        template<>
        struct message_conversion_getter_policy<buffer_vector_type, binary_message> {

            virtual buffer_vector_type get(const binary_message& rhs) {
                return const_cast<binary_message&>(rhs).body()->get();
            }
        };

        template<>
        struct message_conversion_appender_policy<buffer_vector_type, binary_message> {

            virtual void append(binary_message& lhs, const buffer_vector_type& rhs) {
                auto buf_ = gymanstic_convert<buffer_vector_type, buffer_vector_type, buffer_vector_type::value_type>(rhs);
                lhs.body()->append(buf_);
            }
        };

        binary_message& operator<<(binary_message& lhs, const buffer_vector_type& rhs);

        const binary_message& operator >> (const binary_message& lhs, buffer_vector_type& rhs);

        binary_message& operator<<(binary_message& lhs, const std::string& rhs);

        const binary_message& operator >> (const binary_message& lhs, std::string& rhs);

        messaging_api<buffer_vector_type>& operator<<(messaging_api<buffer_vector_type>& lhs, const buffer_vector_type& rhs);

        const messaging_api<buffer_vector_type>& operator >> (const messaging_api<buffer_vector_type>& lhs, buffer_vector_type& rhs);

        messaging_api<buffer_vector_type>& operator<<(messaging_api<buffer_vector_type>& lhs, const std::string& rhs);

        const messaging_api<buffer_vector_type>& operator>>(const messaging_api<buffer_vector_type>& lhs, std::string& rhs);
    }
}

#endif // NNGCPP_MESSAGING_GYMNASTICS_HPP
