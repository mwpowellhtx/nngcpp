#ifndef NNGCPP_ENDPOINT_H
#define NNGCPP_ENDPOINT_H

#include "../nngcpp_integration.h"

namespace nng {

    class endpoint {
        protected:

            endpoint();

        public:

            virtual ~endpoint();
    };
}

#endif // NNGCPP_ENDPOINT_H
