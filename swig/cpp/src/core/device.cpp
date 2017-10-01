#include "socket.h"
#include "device.h"

#include <thread>

namespace nng {

    device_path::device_path(socket* const asockp, socket* const bsockp, bool shouldClose)
        : _asockp(asockp)
            , _bsockp(bsockp)
            , _shouldClose(shouldClose) {
    }

    device_path::~device_path() {
        if (!_shouldClose) { return; }
        if (_asockp) { _asockp->close(); }
        if (_bsockp) { _bsockp->close(); }
    }

    void install_device_sockets_callback(const device_path* const dpp) {
        ::nng_device(dpp->_asockp->sid, dpp->_bsockp->sid);
    }

    device::device(socket* const asockp, socket* const bsockp, bool shouldCloseSockets)
        : thread(nullptr)
            , _pathp(std::make_unique<device_path>(asockp, bsockp, shouldCloseSockets)) {

        // Install the Device and Re-Join the Installer Thread.
        std::thread installer(nng::install_device_sockets_callback, _pathp.get());
        installer.join();
    }

    device::~device() {

        /* Which closes each Device, but does not actually delete the Socket itself, especially
        the event this was still attached to a smart pointer in another scope. */
        _pathp.release();

        // TODO: TBD: is there anything else to "destroy" at this time?
    }
}
