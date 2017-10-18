#ifndef NNGCPP_MESSENGER_H
#define NNGCPP_MESSENGER_H

#define NNG_ONLY
#include <nngcpp.h>

#include <cstddef>

namespace nng {

    class messenger {
    protected:

        messenger();

        virtual ~messenger();
    };
}

#endif // NNGCPP_MESSENGER_H
