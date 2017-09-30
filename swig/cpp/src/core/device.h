#ifndef NNGCPP_DEVICE_H
#define NNGCPP_DEVICE_H

#include <memory>

namespace nng {

    class socket;

    // TODO: TBD: should not expose device_path via SWIG.
    struct NNGCPP_DECLSPEC device_path {
        socket* const _asockp;
        socket* const _bsockp;
        bool _shouldClose;

        device_path(socket* const asockp, socket* const bsockp, bool shouldClose);

        virtual ~device_path();
    };

    class device {
        private:

            void* thread;

            std::unique_ptr<device_path> _pathp;

        public:

            device(socket* const asockp, socket* const bsockp, bool shouldCloseSockets);

            virtual ~device();
    };
}

#endif // NNGCPP_DEVICE_H
