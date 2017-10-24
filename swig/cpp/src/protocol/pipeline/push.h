#ifndef CPPNNG_PROT_PUSH_H
#define CPPNNG_PROT_PUSH_H

#include "../../core/socket.h"

namespace nng {

    namespace protocol {

        namespace v0 {
            
            class push_socket : public socket {
            public:

                push_socket();

                virtual ~push_socket();

            protected:

                virtual std::unique_ptr<binary_message> receive(flag_type flags = flag_none) override;
                virtual bool try_receive(binary_message* const bmp, flag_type flags = flag_none) override;

                virtual buffer_vector_type receive(size_type& sz, flag_type flags = flag_none) override;
                virtual bool try_receive(buffer_vector_type* const bufp, size_type& sz, flag_type flags = flag_none) override;
            };
        }

        typedef v0::push_socket latest_push_socket;
    }
}

#endif // CPPNNG_PROT_PUSH_H
