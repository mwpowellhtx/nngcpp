#ifndef CPPNNG_PROT_RESPOND_H
#define CPPNNG_PROT_RESPOND_H

#include "../../core/socket.h"

namespace nng {

    namespace protocol {

        namespace v0 {
            
            class NNGCPP_DECLSPEC respond_socket : public socket {
                public:

                    respond_socket();

                    virtual ~respond_socket();
            };
        }

        typedef v0::respond_socket respond_socket;
    }
}

#endif // CPPNNG_PROT_RESPOND_H
