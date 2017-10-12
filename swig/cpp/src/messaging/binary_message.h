#ifndef NNGCPP_BINARY_MESSAGE_H
#define NNGCPP_BINARY_MESSAGE_H

#include "binary_message_header.h"
#include "binary_message_body.h"

#include <string>

namespace nng {

    namespace messaging {

        // TODO: TBD: of course they are all "binary" messages close to the wire; however, we may shim a string-based API around this one...
        class binary_message : public message_base {
        private:

            friend class socket;

            typedef messaging::message_base::size_type size_type;

            binary_message_header _header;

            binary_message_body _body;

        protected:

            binary_message(::nng_msg* msgp);

        public:

            binary_message();

            virtual ~binary_message();

            binary_message_header* const header();

            binary_message_body* const body();

            virtual size_type get_size() const override;

            virtual void clear() override;

            virtual void set_msgp(::nng_msg* msgp) override;

            virtual void allocate(size_type sz = 0);

        protected:

            virtual void free();

        public:

            operator std::string();
        };
    }
}

#endif // NNGCPP_BINARY_MESSAGE_H
