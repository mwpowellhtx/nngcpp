#ifndef CPPNNG_PROT_REP_H
#define CPPNNG_PROT_REP_H

#include "../../core/socket.h"

namespace nng {

    namespace protocol {

        namespace v0 {
            
            class NNGCPP_DECLSPEC rep_socket : public socket {
                public:

                    rep_socket();

                    virtual ~rep_socket();
            };
        }

        typedef v0::rep_socket rep_socket;
    }
}

#endif // CPPNNG_PROT_REP_H
