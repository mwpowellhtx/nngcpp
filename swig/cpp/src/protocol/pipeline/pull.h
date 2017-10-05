#ifndef CPPNNG_PROT_PULL_H
#define CPPNNG_PROT_PULL_H

#include "../../core/socket.h"

namespace nng {

    namespace protocol {

        namespace v0 {
            
            class pull_socket : public socket {
                public:

                    pull_socket();

                    virtual ~pull_socket();
            };
        }

        typedef v0::pull_socket latest_pull_socket;
    }
}

#endif // CPPNNG_PROT_PULL_H
