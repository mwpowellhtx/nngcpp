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

#include "message_base.h"

#include "messaging_api_base.hpp"

#include <cstdint>
#include <vector>

namespace nng {

    namespace messaging {
 
#ifndef NNGCPP_STRING_BASED_MESSAGE_H
        class binary_message;
#endif //NNGCPP_STRING_BASED_MESSAGE_H

        class binary_message_body : public messaging_api<message_base::buffer_vector_type> {
        protected:

            friend class binary_message;

            binary_message_body();

            binary_message_body(::nng_msg* msgp);

        public:

            virtual ~binary_message_body();

            virtual const buffer_vector_type get() const override;

            virtual size_type get_size() const override;

            virtual void clear() override;

            virtual void append(const buffer_vector_type& buf) override;

            virtual void insert(const buffer_vector_type& buf) override;

            virtual void trim(size_type sz) override;

            virtual void chop(size_type sz) override;
        };
    }
}

#endif // NNGCPP_BINARY_MESSAGE_BODY_H
