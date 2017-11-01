#ifndef NNGCPP_CAN_LISTEN_HPP
#define NNGCPP_CAN_LISTEN_HPP

#include "enums.h"

#include <string>

namespace nng {

#ifndef NNGCPP_LISTENER_H
    class _Listener;
#endif // NNGCPP_LISTENER_H

    // This is an instance where the internal externally facing API really does need to be pointer instead of reference-based.
    struct ICanListen {
        virtual void Listen(const std::string& addr, flag_type flags = flag_none) = 0;
        virtual void Listen(const std::string& addr, _Listener* const lp, flag_type flags = flag_none) = 0;
    };
}

#endif // NNGCPP_CAN_LISTEN_HPP
