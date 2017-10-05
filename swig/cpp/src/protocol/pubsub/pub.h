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
            };
        }

        typedef v0::pub_socket latest_pub_socket;
    }
}

#endif // CPPNNG_PROT_PUB_H
