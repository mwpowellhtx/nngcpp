#ifndef NNGCPP_CAN_CLOSE_HPP
#define NNGCPP_CAN_CLOSE_HPP

namespace nng {

    struct ICanClose {
        virtual void Close() = 0;
    };
}

#endif // NNGCPP_CAN_CLOSE_HPP
