#ifndef CPPNNG_PROT_PUB_H
#define CPPNNG_PROT_PUB_H

#include "../../core/socket.h"

namespace nng {

    namespace protocol {

        namespace v0 {
            
            class pub_socket : public _Socket {
            public:

                pub_socket();

                virtual ~pub_socket();

            protected:

                virtual std::unique_ptr<binary_message> Receive(flag_type flags = flag_none) override;
                virtual bool TryReceive(binary_message* const bmp, flag_type flags = flag_none) override;

                virtual buffer_vector_type Receive(size_type& sz, flag_type flags = flag_none) override;
                virtual bool TryReceive(buffer_vector_type* const bufp, size_type& sz, flag_type flags = flag_none) override;

                virtual void ReceiveAsync(basic_async_service* const svcp) override;
            };
        }

        typedef v0::pub_socket latest_pub_socket;
    }
}

#endif // CPPNNG_PROT_PUB_H
