#ifndef NNGCPP_ENDPOINT_H
#define NNGCPP_ENDPOINT_H

#include "enums.h"

#include "having_one.hpp"
#include "can_close.hpp"

namespace nng {

    // TODO: TBD: carries along with it information about ::nng_pipe ... just a matter of how to present it to the C++ (or SWIG) community...
    class endpoint : public having_one, public can_close {
        protected:

            endpoint();

        public:

            virtual ~endpoint();

            virtual void start(flag_type flags = flag_none) = 0;
    };
}

#endif // NNGCPP_ENDPOINT_H
