#ifndef NNGCPP_BINARY_MESSAGE_H
#define NNGCPP_BINARY_MESSAGE_H

#include "../core/types.h"

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

            ::nng_msg* _msgp;

            binary_message_header _header;

            binary_message_body _body;

        public:

            binary_message();

            binary_message(size_type sz);

            binary_message(::nng_msg* msgp);

            virtual ~binary_message();

            binary_message_header* const header();

            binary_message_body* const body();

            virtual ::nng_msg* get_msgp() override;

            virtual void set_msgp(::nng_msg* msgp);

            virtual void on_sent();

            virtual size_type get_size();

            virtual void clear() override;

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
