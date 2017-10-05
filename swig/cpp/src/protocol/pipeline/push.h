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
            };
        }

        typedef v0::push_socket latest_push_socket;
    }
}

#endif // CPPNNG_PROT_PUSH_H
