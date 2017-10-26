#include "device.h"
#include "socket.h"

namespace nng {

    device_path::device_path(socket* const asockp, socket* const bsockp, bool shouldClose)
        : _asockp(asockp)
            , _bsockp(bsockp)
            , _shouldClose(shouldClose) {
    }

    device_path::~device_path() {
        if (!_shouldClose) { return; }
        if (_asockp) { _asockp->Close(); }
        if (_bsockp) { _bsockp->Close(); }
    }

    void install_device_sockets_callback(const device_path* const dpp) {
        ::nng_device(dpp->_asockp->sid, dpp->_bsockp->sid);
    }

    device::device(socket* const asockp, socket* const bsockp, bool shouldCloseSockets)
        : _pathp(std::make_unique<device_path>(asockp, bsockp, shouldCloseSockets))
            , _threadp(std::make_unique<std::thread>(nng::install_device_sockets_callback, _pathp.get())) {
    }

    device::~device() {

        /* Which closes each component involved in the Device, but does not actually delete
        the Socket itself, especially the event this was still attached to a smart pointer
        in another scope. */

        _pathp.release();

        // Then we should be able to re-join the thread.
        _threadp->join();
    }
}
