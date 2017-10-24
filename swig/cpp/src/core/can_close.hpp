#ifndef NNGCPP_CAN_CLOSE_HPP
#define NNGCPP_CAN_CLOSE_HPP

namespace nng {

    class can_close {
    public:
        virtual void close() = 0;
    };
}

#endif // NNGCPP_CAN_CLOSE_HPP
