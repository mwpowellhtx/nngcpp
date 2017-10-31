#ifndef CPPNNG_PROT_SURVEY_H
#define CPPNNG_PROT_SURVEY_H

#include "../../core/socket.h"

namespace nng {

    namespace protocol {

        namespace v0 {
            
            class survey_socket : public _Socket {
                public:

                    survey_socket();

                    virtual ~survey_socket();
            };
        }

        typedef v0::survey_socket latest_survey_socket;
    }
}

#endif // CPPNNG_PROT_SURVEY_H
