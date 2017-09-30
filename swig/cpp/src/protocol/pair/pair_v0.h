#ifndef CPPNNG_PROT_PAIR_V0_H
#define CPPNNG_PROT_PAIR_V0_H

#include "../../core/socket.h"

namespace nng {

    namespace protocol {
        
        namespace v0 {
            
            class NNGCPP_DECLSPEC pair_socket : public socket {
                public:

                    pair_socket();

                    virtual ~pair_socket();
            };
        }

        typedef v0::pair_socket pair_socket;
    }
}

#endif // CPPNNG_PROT_PAIR_V0_H
