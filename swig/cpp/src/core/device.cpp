#include "nng.h"
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
        if (_asockp) { _asockp->close(); }
        if (_bsockp) { _bsockp->close(); }
    }

    void __cdecl thread_callback(void* arg) {
        auto* const dp = static_cast<device_path* const>(arg);
        ::nng_device(dp->_asockp->sid, dp->_bsockp->sid);
    }

    device::device(socket* const asockp, socket* const bsockp, bool shouldCloseSockets)
        : thread(nullptr)
            , _pathp(std::make_unique<device_path>(asockp, bsockp, shouldCloseSockets)) {

        auto ec = ::nng_thread_create(&thread, &nng::thread_callback, static_cast<void*>(_pathp.get()));
    }

    device::~device() {

        /* Which closes each Device, but does not actually delete the Socket itself, especially
        the event this was still attached to a smart pointer in another scope. */
        _pathp.release();

        ::nng_thread_destroy(thread);
    }
}
