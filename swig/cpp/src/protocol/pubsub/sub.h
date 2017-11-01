#ifndef CPPNNG_PROT_SUB_H
#define CPPNNG_PROT_SUB_H

#include "../../core/socket.h"

namespace nng {

    namespace protocol {

        namespace v0 {
            
            class sub_socket : public _Socket {
            public:

                sub_socket();

                virtual ~sub_socket();

            protected:

                virtual void Send(binary_message& m, flag_type flags = flag_none) override;

                virtual void Send(const buffer_vector_type& buf, flag_type flags = flag_none) override;
                virtual void Send(const buffer_vector_type& buf, size_type sz, flag_type flags = flag_none) override;

                virtual void SendAsync(const basic_async_service* const svcp) override;
            };
        }

        typedef v0::sub_socket latest_sub_socket;
    }
}

#endif // CPPNNG_PROT_SUB_H
