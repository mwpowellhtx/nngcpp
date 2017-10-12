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
#include <vector>

namespace nng {

    namespace messaging {

#ifndef NNGCPP_BINARY_MESSAGE_H
        class binary_message;
#endif //NNGCPP_BINARY_MESSAGE_H

        // TODO: TBD: potentially handling these as a string-based as well...
        class binary_message_header : public readonly_messaging_api<message_base::buffer_vector_type> {
        public:

            typedef readonly_messaging_api<message_base::buffer_vector_type>::size_type size_type;
            typedef readonly_messaging_api<message_base::buffer_vector_type>::buffer_vector_type buffer_vector_type;

        protected:

            friend class binary_message;

            binary_message_header();

            binary_message_header(::nng_msg* msgp);

        public:

            virtual ~binary_message_header();

            virtual bool try_get(buffer_vector_type& value) const override;

            virtual message_base::size_type get_size() const override;

            // TODO: TBD: so if header is truly "read-only" then it is debatable whether "clear" should be exposed via header...
            virtual void clear() override;

            virtual bool has_message() const override;

        //    // TODO: TBD: technically we should not be doing anything with header, per se; however, that being said, future efforts may involve moving from msg/header/body to "scatter/gather" pattern.
        //protected:

        //    virtual void append(const message_base::buffer_vector_type& buf) override;

        //    virtual void insert(const message_base::buffer_vector_type& buf) override;

        //    virtual void trim(size_type sz) override;

        //    virtual void chop(size_type sz) override;
        };
    }
}

#endif // NNGCPP_MESSAGE_HEADER_H
