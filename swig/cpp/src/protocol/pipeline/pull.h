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

                virtual void Send(binary_message* const bmp, flag_type flags = flag_none) override;

                virtual void Send(const buffer_vector_type* const bufp, flag_type flags = flag_none) override;
                virtual void Send(const buffer_vector_type* const bufp, size_type sz, flag_type flags = flag_none) override;

                virtual void SendAsync(const basic_async_service* const svcp) override;
            };
        }

        typedef v0::pull_socket latest_pull_socket;
    }
}

#endif // CPPNNG_PROT_PULL_H
