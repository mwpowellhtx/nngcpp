#ifndef CPPNNG_PROT_PULL_H
#define CPPNNG_PROT_PULL_H

#include "../../core/socket.h"

namespace nng {

    namespace protocol {

        namespace v0 {

            class pull_socket : public _Socket {
            public:

                pull_socket();

                virtual ~pull_socket();

            protected:

                virtual void Send(binary_message& m, flag_type flags = flag_none) override;

                virtual void Send(const buffer_vector_type& buf, flag_type flags = flag_none) override;
                virtual void Send(const buffer_vector_type& buf, size_type sz, flag_type flags = flag_none) override;

                virtual void SendAsync(const basic_async_service* const svcp) override;
            };
        }

        typedef v0::pull_socket latest_pull_socket;
    }
}

#endif // CPPNNG_PROT_PULL_H
