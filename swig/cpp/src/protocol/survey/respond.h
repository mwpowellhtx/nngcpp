#ifndef CPPNNG_PROT_RESPOND_H
#define CPPNNG_PROT_RESPOND_H

#include "../../core/socket.h"

namespace nng {

    namespace protocol {

        namespace v0 {
            
            class respond_socket : public _Socket {
                public:

                    respond_socket();

                    virtual ~respond_socket();
            };
        }

        typedef v0::respond_socket latest_respond_socket;
    }
}

#endif // CPPNNG_PROT_RESPOND_H
