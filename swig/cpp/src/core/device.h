#ifndef NNGCPP_DEVICE_H
#define NNGCPP_DEVICE_H

#include "../nngcpp_integration.h"

#include <memory>
#include <thread>

namespace nng {

    class socket;

    // TODO: TBD: should perhaps not expose device_path as an external resource, much less to SWIG
    struct device_path {
        socket* const _asockp;
        socket* const _bsockp;
        bool _shouldClose;

        device_path(socket* const asockp, socket* const bsockp, bool shouldClose);

        virtual ~device_path();
    };

    class device {
        private:

            std::unique_ptr<device_path> _pathp;

            std::unique_ptr<std::thread> _threadp;

        public:

            device(socket* const asockp, socket* const bsockp, bool shouldCloseSockets);

            virtual ~device();
    };
}

#endif // NNGCPP_DEVICE_H
