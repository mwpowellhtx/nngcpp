#ifndef CPPNNG_PROT_PAIR_V0_H
#define CPPNNG_PROT_PAIR_V0_H

#include "../../core/socket.h"

namespace nng {

    namespace protocol {
        
        namespace v0 {
            
            class pair_socket : public _Socket {
                public:

                    pair_socket();

                    virtual ~pair_socket();
            };
        }

        //// TODO: TBD: will assume that v1 is the appropriate version for the time being.
        //typedef v0::pair_socket latest_pair_socket;
    }
}

#endif // CPPNNG_PROT_PAIR_V0_H
