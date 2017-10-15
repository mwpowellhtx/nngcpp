#ifndef NNGCPP_BINARY_MESSAGE_H
#define NNGCPP_BINARY_MESSAGE_H

#include "binary_message_header.h"
#include "binary_message_body.h"

#include <string>

namespace nng {

    namespace messaging {

#ifndef NNGCPP_MESSAGE_PIPE_H
        class message_pipe;
#endif //NNGCPP_MESSAGE_PIPE_H

        // TODO: TBD: of course they are all "binary" messages close to the wire; however, we may shim a string-based API around this one...
        class binary_message : public message_base {
        private:

            friend class socket;

            typedef messaging::message_base::size_type size_type;

            binary_message_header _header;

            binary_message_body _body;

        protected:

            binary_message(::nng_msg* msgp);

            binary_message(::nng_msg* msgp, size_type sz);

        public:

            binary_message();

            binary_message(size_type sz);

            virtual ~binary_message();

            binary_message_header* const header();

            binary_message_body* const body();

            virtual size_type get_size();

            virtual void clear() override;

            virtual void set_msgp(::nng_msg* msgp) override;

            virtual void allocate(size_type sz = 0);

            virtual void set_pipe(const message_pipe* const mpp);

        protected:

            virtual void free();

        public:

            operator std::string();
        };
    }
}

#endif // NNGCPP_BINARY_MESSAGE_H
