#ifndef NNGCPP_ICANDIAL_H
#define NNGCPP_ICANDIAL_H

#include "enums.h"

#include <string>

namespace nng {

#ifndef NNGCPP_DIALER_H
    class _Dialer;
#endif // NNGCPP_DIALER_H

    struct ICanDial {

        virtual ~ICanDial();

        virtual void Dial(const std::string& addr, flag_type flags = flag_none) = 0;
        virtual void Dial(const std::string& addr, _Dialer* const dp, flag_type flags = flag_none) = 0;

    protected:

        ICanDial();
    };
}

#endif // NNGCPP_ICANDIAL_H
