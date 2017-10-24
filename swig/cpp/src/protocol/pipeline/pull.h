#ifndef CPPNNG_PROT_PULL_H
#define CPPNNG_PROT_PULL_H

#include "../../core/socket.h"

namespace nng {

    namespace protocol {

        namespace v0 {
            
            class pull_socket : public socket {
            public:

                pull_socket();
                
                virtual ~pull_socket();

            protected:

                virtual void send(binary_message* const bmp, flag_type flags = flag_none) override;

                virtual void send(const buffer_vector_type* const bufp, flag_type flags = flag_none) override;
                virtual void send(const buffer_vector_type* const bufp, size_type sz, flag_type flags = flag_none) override;
            };
        }

        typedef v0::pull_socket latest_pull_socket;
    }
}

#endif // CPPNNG_PROT_PULL_H
