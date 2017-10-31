#ifndef NNGCPP_DEVICE_H
#define NNGCPP_DEVICE_H

#include "socket.h"

#include <memory>
#include <thread>

namespace nng {

    // TODO: TBD: should perhaps not expose device_path as an external resource, much less to SWIG
    struct device_path {
        _Socket* const _asockp;
        _Socket* const _bsockp;
        bool _shouldClose;

        device_path(_Socket* const asockp, _Socket* const bsockp, bool shouldClose);

        virtual ~device_path();
    };

    class device {
        private:

            std::unique_ptr<device_path> _pathp;

            std::unique_ptr<std::thread> _threadp;

        public:

            device(_Socket* const asockp, _Socket* const bsockp, bool shouldCloseSockets);

            virtual ~device();
    };
}

#endif // NNGCPP_DEVICE_H
