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

#include "message_base.h"

#include "messaging_api_base.hpp"

#include <cstdint>

namespace nng {

    namespace messaging {

#ifndef NNGCPP_BINARY_MESSAGE_H
        class binary_message;
#endif //NNGCPP_BINARY_MESSAGE_H

        // TODO: TBD: potentially handling these as a string-based as well...
        class binary_message_header
            : public message_base
            , public supports_get_api<message_base::buffer_vector_type>
            , public supports_append_api<message_base::buffer_vector_type, uint32_t>
            , public supports_insert_api<message_base::buffer_vector_type, uint32_t>
            , public supports_chop_api<message_base::size_type, uint32_t*>
            , public supports_trim_api<message_base::size_type, uint32_t*> {
        public:

            typedef message_base::size_type size_type;
            typedef message_base::buffer_vector_type buffer_vector_type;

        protected:

            friend class binary_message;

            binary_message_header();

            binary_message_header(::nng_msg* msgp);

        public:

            virtual ~binary_message_header();

            virtual ::nng_msg* get_msgp() const override;

            virtual bool try_get(buffer_vector_type& value) const override;

            virtual message_base::size_type get_size() const override;

            // TODO: TBD: so if header is truly "read-only" then it is debatable whether "clear" should be exposed via header...
            virtual void clear() override;

            virtual void append(const uint32_t& val);

            virtual void insert(const uint32_t& val);

            virtual void trim(uint32_t* valp);

            virtual void chop(uint32_t* valp);

        protected:

            virtual void append(const buffer_vector_type& buf);

            virtual void insert(const buffer_vector_type& buf);

            virtual void trim(size_type sz);

            virtual void chop(size_type sz);
        };
    }
}

#endif // NNGCPP_MESSAGE_HEADER_H
