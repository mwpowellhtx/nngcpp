#ifndef NNGCPP_ENDPOINT_H
#define NNGCPP_ENDPOINT_H

#include "../nngcpp_integration.h"

namespace nng {

    // TODO: TBD: carries along with it information about ::nng_pipe ... just a matter of how to present it to the C++ (or SWIG) community...
    class endpoint {
        protected:

            endpoint();

        public:

            virtual ~endpoint();
    };
}

#endif // NNGCPP_ENDPOINT_H
