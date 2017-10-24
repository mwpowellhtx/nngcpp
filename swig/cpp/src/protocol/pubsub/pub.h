#ifndef CPPNNG_PROT_PUB_H
#define CPPNNG_PROT_PUB_H

#include "../../core/socket.h"

namespace nng {

    namespace protocol {

        namespace v0 {
            
            class pub_socket : public socket {
            public:

                pub_socket();

                virtual ~pub_socket();

            protected:

                virtual std::unique_ptr<binary_message_type> receive(flag_type flags = flag_none) override;
                virtual bool try_receive(binary_message_type* const bmp, flag_type flags = flag_none) override;

                virtual buffer_vector_type receive(size_type& sz, flag_type flags = flag_none) override;
                virtual bool try_receive(buffer_vector_type* const bufp, size_type& sz, flag_type flags = flag_none) override;
            };
        }

        typedef v0::pub_socket latest_pub_socket;
    }
}

#endif // CPPNNG_PROT_PUB_H
