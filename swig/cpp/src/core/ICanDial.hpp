#ifndef NNGCPP_CAN_DIAL_HPP
#define NNGCPP_CAN_DIAL_HPP

#include "enums.h"

#include <string>

namespace nng {

#ifndef NNGCPP_DIALER_H
    class _Dialer;
#endif // NNGCPP_DIALER_H

    // This is an instance where the internal externally facing API really does need to be pointer instead of reference-based.
    struct ICanDial {
        virtual void Dial(const std::string& addr, flag_type flags = flag_none) = 0;
        virtual void Dial(const std::string& addr, _Dialer* const dp, flag_type flags = flag_none) = 0;
    };
}

#endif // NNGCPP_CAN_DIAL_HPP
