#ifndef CPPNNG_PROT_PUB_H
#define CPPNNG_PROT_PUB_H

#include "../../core/socket.h"

namespace nng {

    namespace protocol {

        namespace v0 {
            
            class NNGCPP_DECLSPEC pub_socket : public socket {
                public:

                    pub_socket();

                    virtual ~pub_socket();
            };
        }

        typedef v0::pub_socket pub_socket;
    }
}

#endif // CPPNNG_PROT_PUB_H
