#ifndef NNGCPP_CAN_SHUTDOWN_HPP
#define NNGCPP_CAN_SHUTDOWN_HPP

namespace nng {

    struct ICanShutdown {
        virtual void Shutdown() = 0;
    };
}

#endif // NNGCPP_CAN_SHUTDOWN_HPP
