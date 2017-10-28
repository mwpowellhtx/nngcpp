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

#include "messaging_api_base.hpp"

#include <string>
#include <algorithm>

namespace nng {

    template<class Target_, class Message_>
    struct message_conversion_getter_policy {

        typedef Target_ target_type;
        typedef Message_ message_type;
        typedef message_conversion_getter_policy<Target_, Message_> base_type;

        virtual target_type Get(message_type& rhs) {
            return rhs.Get();
        }
    };

    // TODO: TBD: I wonder whether I can nix the policy template specializations altogether?
    template<class Target_, class Message_>
    struct message_conversion_appender_policy {

        typedef Target_ target_type;
        typedef Message_ message_type;
        typedef message_conversion_appender_policy<Target_, Message_> base_type;

        virtual void Append(message_type& lhs, const target_type& rhs) {
            lhs.Append(rhs);
        }
    };

    template<>
    struct message_conversion_getter_policy<buffer_vector_type, ISupportsGet<buffer_vector_type>> {

        virtual buffer_vector_type Get(ISupportsGet<buffer_vector_type>& rhs) {
            return rhs.Get();
        }
    };

    template<>
    struct message_conversion_appender_policy<buffer_vector_type, ISupportsAppend<buffer_vector_type>> {

        virtual void Append(ISupportsAppend<buffer_vector_type>& lhs, const buffer_vector_type& rhs) {
            lhs.Append(rhs);
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
    struct message_conversion_getter_policy<std::string, ISupportsGet<buffer_vector_type>> {

        virtual std::string Get(ISupportsGet<buffer_vector_type>& rhs) {
            auto lhs_ = rhs.Get();
            return gymanstic_convert<buffer_vector_type, std::string, std::string::value_type>(lhs_);
        }
    };

    template<>
    struct message_conversion_appender_policy<std::string, ISupportsAppend<buffer_vector_type>> {

        virtual void Append(ISupportsAppend<buffer_vector_type>& lhs, const std::string& rhs) {
            auto buf_ = gymanstic_convert<std::string, buffer_vector_type, buffer_vector_type::value_type>(rhs);
            lhs.Append(buf_);
        }
    };

    template<class Body_, class Header_>
    struct message_conversion_getter_policy<std::string, _BasicMessage<Body_, Header_>> {

        virtual std::string Get(binary_message& rhs) {
            const auto lhs_ = const_cast<binary_message&>(rhs).body()->Get();
            return gymanstic_convert<buffer_vector_type, std::string, std::string::value_type>(lhs_);
        }
    };

    template<class Body_, class Header_>
    struct message_conversion_appender_policy<std::string, _BasicMessage<Body_, Header_>> {

        virtual void Append(binary_message& lhs, const std::string& rhs) {
            const auto buf_ = gymanstic_convert<std::string, buffer_vector_type, buffer_vector_type::value_type>(rhs);
            lhs.body()->Append(buf_);
        }
    };

    template<class Body_, class Header_>
    struct message_conversion_getter_policy<buffer_vector_type, _BasicMessage<Body_, Header_>> {

        virtual buffer_vector_type Get(binary_message& rhs) {
            return const_cast<binary_message&>(rhs).body()->Get();
        }
    };

    template<class Body_, class Header_>
    struct message_conversion_appender_policy<buffer_vector_type, _BasicMessage<Body_, Header_>> {

        virtual void Append(binary_message& lhs, const buffer_vector_type& rhs) {
            auto buf_ = gymanstic_convert<buffer_vector_type, buffer_vector_type, buffer_vector_type::value_type>(rhs);
            lhs.body()->Append(buf_);
        }
    };

    template<>
    struct message_conversion_getter_policy<std::string, binary_message> {

        virtual std::string Get(binary_message& rhs) {
            const auto lhs_ = const_cast<binary_message&>(rhs).body()->Get();
            return gymanstic_convert<buffer_vector_type, std::string, std::string::value_type>(lhs_);
        }
    };

    template<>
    struct message_conversion_appender_policy<std::string, binary_message> {

        virtual void Append(binary_message& lhs, const std::string& rhs) {
            const auto buf_ = gymanstic_convert<std::string, buffer_vector_type, buffer_vector_type::value_type>(rhs);
            lhs.body()->Append(buf_);
        }
    };

    template<>
    struct message_conversion_getter_policy<buffer_vector_type, binary_message> {

        virtual buffer_vector_type Get(binary_message& rhs) {
            return const_cast<binary_message&>(rhs).body()->Get();
        }
    };

    template<>
    struct message_conversion_appender_policy<buffer_vector_type, binary_message> {

        virtual void Append(binary_message& lhs, const buffer_vector_type& rhs) {
            auto buf_ = gymanstic_convert<buffer_vector_type, buffer_vector_type, buffer_vector_type::value_type>(rhs);
            lhs.body()->Append(buf_);
        }
    };

    template<class Body_, class Header_>
    binary_message& operator<<(_BasicMessage<Body_, Header_>& lhs, const buffer_vector_type& rhs) {
        auto ops = message_conversion_appender_policy<buffer_vector_type, binary_message>();
        ops.Append(lhs, rhs);
        return lhs;
    }

    template<class Body_, class Header_>
    binary_message& operator >> (_BasicMessage<Body_, Header_>& lhs, buffer_vector_type& rhs) {
        auto ops = message_conversion_getter_policy<buffer_vector_type, binary_message>();
        rhs = ops.Get(lhs);
        return lhs;
    }

    template<class Body_, class Header_>
    binary_message& operator<<(_BasicMessage<Body_, Header_>& lhs, const std::string& rhs) {
        auto ops = message_conversion_appender_policy<std::string, binary_message>();
        ops.Append(lhs, rhs);
        return lhs;
    }

    template<class Body_, class Header_>
    binary_message& operator >> (_BasicMessage<Body_, Header_>& lhs, std::string& rhs) {
        auto ops = message_conversion_getter_policy<std::string, binary_message>();
        rhs = ops.Get(lhs);
        return lhs;
    }

    binary_message& operator<<(binary_message& lhs, const buffer_vector_type& rhs);

    binary_message& operator >> (binary_message& lhs, buffer_vector_type& rhs);

    binary_message& operator<<(binary_message& lhs, const std::string& rhs);

    binary_message& operator >> (binary_message& lhs, std::string& rhs);

    ISupportsAppend<buffer_vector_type>& operator<<(ISupportsAppend<buffer_vector_type>& lhs, const buffer_vector_type& rhs);

    ISupportsGet<buffer_vector_type>& operator >> (ISupportsGet<buffer_vector_type>& lhs, buffer_vector_type& rhs);

    ISupportsAppend<buffer_vector_type>& operator<<(ISupportsAppend<buffer_vector_type>& lhs, const std::string& rhs);

    ISupportsGet<buffer_vector_type>& operator >> (ISupportsGet<buffer_vector_type>& lhs, std::string& rhs);
}

#endif // NNGCPP_MESSAGING_GYMNASTICS_HPP
