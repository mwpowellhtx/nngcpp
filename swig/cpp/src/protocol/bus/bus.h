#ifndef CPPNNG_PROT_BUS_H
#define CPPNNG_PROT_BUS_H

#include "../../core/socket.h"

namespace nng {

    namespace protocol {

        namespace v0 {
            
            class NNGCPP_DECLSPEC bus_socket : public socket {
                public:

                    bus_socket();

                    virtual ~bus_socket();
            };
        }

        typedef v0::bus_socket bus_socket;
    }
}

#endif // CPPNNG_PROT_BUS_H
