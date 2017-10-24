#ifndef CPPNNG_PROT_SUB_H
#define CPPNNG_PROT_SUB_H

#include "../../core/socket.h"

namespace nng {

    namespace protocol {

        namespace v0 {
            
            class sub_socket : public socket {
            public:

                sub_socket();

                virtual ~sub_socket();

            protected:

                virtual void send(binary_message_type* const bmp, flag_type flags = flag_none) override;

                virtual void send(const buffer_vector_type* const bufp, flag_type flags = flag_none) override;
                virtual void send(const buffer_vector_type* const bufp, size_type sz, flag_type flags = flag_none) override;
            };
        }

        typedef v0::sub_socket latest_sub_socket;
    }
}

#endif // CPPNNG_PROT_SUB_H
