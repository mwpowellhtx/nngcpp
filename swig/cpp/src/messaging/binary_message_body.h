//
// Copyright (c) 2017 Michel W Powell <mwpowellhtx@gmail.com>
//
// This software is supplied under the terms of the MIT License, a
// copy of which should be located in the distribution where this
// file was obtained (LICENSE.txt).  A copy of the license may also be
// found online at https://opensource.org/licenses/MIT.
//

#ifndef NNGCPP_BINARY_MESSAGE_BODY_H
#define NNGCPP_BINARY_MESSAGE_BODY_H

#include "../core/types.h"

#include "message_base.h"

#include "messaging_api_base.hpp"

#include <cstdint>

namespace nng {

    namespace messaging {
 
#ifndef NNGCPP_STRING_BASED_MESSAGE_H
        class binary_message;
#endif //NNGCPP_STRING_BASED_MESSAGE_H

        class binary_message_body
            : public message_part
            , public supports_get_api<buffer_vector_type>
            , public supports_append_api<buffer_vector_type, uint32_t>
            , public supports_insert_api<buffer_vector_type, uint32_t>
            , public supports_chop_api<uint32_t, uint32_t*>
            , public supports_trim_api<uint32_t, uint32_t*> {
        protected:

            friend class binary_message;

            binary_message_body(message_base* const basep);

        public:

            virtual ~binary_message_body();

            virtual bool try_get(buffer_vector_type& value) override;

            virtual size_type get_size() override;

        public:

            virtual void clear() override;

            virtual void append(const buffer_vector_type& buf);

            virtual void insert(const buffer_vector_type& buf);

            virtual void trim(size_type sz);

            virtual void chop(size_type sz);

            virtual void append(const uint32_t& val);

            virtual void insert(const uint32_t& val);

            virtual void trim(uint32_t* valp);

            virtual void chop(uint32_t* valp);
        };
    }
}

#endif // NNGCPP_BINARY_MESSAGE_BODY_H
