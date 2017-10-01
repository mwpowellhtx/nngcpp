#ifndef CPPNNG_PROT_REQ_H
#define CPPNNG_PROT_REQ_H

#include "../../core/socket.h"

namespace nng {

    namespace protocol {

        namespace v0 {
            
            class req_socket : public socket {
                public:

                    req_socket();

                    virtual ~req_socket();
            };
        }

        typedef v0::req_socket req_socket;
    }
}

#endif // CPPNNG_PROT_REQ_H
