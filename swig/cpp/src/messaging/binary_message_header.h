//
// Copyright (c) 2017 Michel W Powell <mwpowellhtx@gmail.com>
//
// This software is supplied under the terms of the MIT License, a
// copy of which should be located in the distribution where this
// file was obtained (LICENSE.txt).  A copy of the license may also be
// found online at https://opensource.org/licenses/MIT.
//

#ifndef NNGCPP_MESSAGE_HEADER_H
#define NNGCPP_MESSAGE_HEADER_H

#include "../core/types.h"

#include "message_base.h"

#include "messaging_api_base.hpp"

#include <cstdint>

namespace nng {

    namespace messaging {

#ifndef NNGCPP_BINARY_MESSAGE_H
        template<class Body_, class Header_> class basic_binary_message;
#endif //NNGCPP_BINARY_MESSAGE_H

        // TODO: TBD: potentially handling these as a string-based as well...
        class binary_message_header
            : public message_part
            , public supports_get_api<buffer_vector_type>
            , public supports_append_api<buffer_vector_type, uint32_t>
            , public supports_prepend_api<buffer_vector_type, uint32_t>
            , public supports_ltrim_api<size_type, uint32_t&>
            , public supports_rtrim_api<size_type, uint32_t&> {
        protected:

            template<class Body_, class Header_> friend class basic_binary_message;

            binary_message_header(message_base* const msgp);

        public:

            virtual ~binary_message_header();

            virtual bool try_get(buffer_vector_type& value) override;

            virtual size_type get_size() override;

            // TODO: TBD: so if header is truly "read-only" then it is debatable whether "clear" should be exposed via header...
            virtual void clear() override;

            virtual void append(const uint32_t& val);

            virtual void prepend(const uint32_t& val);

            virtual void ltrim(uint32_t& val);

            virtual void rtrim(uint32_t& val);

        protected:

            virtual void append(const buffer_vector_type& buf);

            virtual void prepend(const buffer_vector_type& buf);

            virtual void ltrim(size_type sz);

            virtual void rtrim(size_type sz);

            virtual void append(const std::string& s);

            virtual void prepend(const std::string& s);
        };
    }
}

#endif // NNGCPP_MESSAGE_HEADER_H
