#ifndef CPPNNG_PROT_SURVEY_H
#define CPPNNG_PROT_SURVEY_H

#include "../../core/socket.h"

namespace nng {

    namespace protocol {

        namespace v0 {
            
            class NNGCPP_DECLSPEC survey_socket : public socket {
                public:

                    survey_socket();

                    virtual ~survey_socket();
            };
        }

        typedef v0::survey_socket survey_socket;
    }
}

#endif // CPPNNG_PROT_SURVEY_H
