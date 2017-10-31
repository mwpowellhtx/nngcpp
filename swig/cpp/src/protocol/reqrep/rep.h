#ifndef CPPNNG_PROT_REP_H
#define CPPNNG_PROT_REP_H

#include "../../core/socket.h"

namespace nng {

    namespace protocol {

        namespace v0 {
            
            class rep_socket : public _Socket {
                public:

                    rep_socket();

                    virtual ~rep_socket();
            };
        }

        typedef v0::rep_socket latest_rep_socket;
    }
}

#endif // CPPNNG_PROT_REP_H
