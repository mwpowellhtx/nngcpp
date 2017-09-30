#ifndef CPPNNG_PROT_SUB_H
#define CPPNNG_PROT_SUB_H

#include "../../core/socket.h"

namespace nng {

    namespace protocol {

        namespace v0 {
            
            class NNGCPP_DECLSPEC sub_socket : public socket {
                public:

                    sub_socket();

                    virtual ~sub_socket();
            };
        }

        typedef v0::sub_socket sub_socket;
    }
}

#endif // CPPNNG_PROT_SUB_H
