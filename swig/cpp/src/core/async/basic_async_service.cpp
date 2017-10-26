#include "basic_async_service.h"
#include "../invocation.hpp"
#include "../../messaging/binary_message.h"

namespace nng {

    using std::bind;
    using std::placeholders::_1;
    using nng::exceptions::invalid_operation;

    basic_async_service::basic_async_service()
        : basic_async_service([]() {}) {
    }

    basic_async_service::basic_async_service(const basic_callback_func& on_cb)
        : _aiop(nullptr), _on_cb(on_cb)
        , _free(), _wait(), _stop(), _cancel()
        , _result(), _set_timeout(), _get_msg(), _set_msg() {

        auto alloc = bind(&::nng_aio_alloc, _1, &basic_async_service::_aoi_cb, this);
        invocation::with_default_error_handling(alloc, &_aiop);
        configure(_aiop);
    }

    basic_async_service::~basic_async_service() {
        invocation::with_void_return_value(_free);
        _aiop = nullptr;
    }

    void basic_async_service::configure(aio_type* const aiop) {
        _free = bind(&::nng_aio_free, aiop);
        _wait = bind(&::nng_aio_wait, aiop);
        _stop = bind(&::nng_aio_stop, aiop);
        _cancel = bind(&::nng_aio_cancel, aiop);
        _result = bind(&::nng_aio_result, aiop);
        _set_timeout = bind(&::nng_aio_set_timeout, aiop, _1);
        _get_msg = bind(&::nng_aio_get_msg, aiop);
        _set_msg = bind(&::nng_aio_set_msg, aiop, _1);
    }

    void basic_async_service::_aoi_cb(void* selfp) {
        // There is not much we can do if it isn't "this".
        auto __selfp = static_cast<basic_async_service*>(selfp);
        if (!__selfp) throw invalid_operation("Invalid asynchronous callback");
        __selfp->_on_cb();
    }

    void basic_async_service::wait() const {
        invocation::with_void_return_value(_wait);
    }
    
    void basic_async_service::stop() const {
        invocation::with_void_return_value(_stop);
    }
    
    void basic_async_service::cancel() const {
        invocation::with_void_return_value(_cancel);
    }

    bool basic_async_service::success() const {
        invocation::with_default_error_handling(_result);
        return true;
    }

    bool basic_async_service::try_success() const {
        try {
            return success();
        }
        catch (...) {
            // TODO: TBD: may log the exception here...
            return false;
        }
    }

    void basic_async_service::set(const duration_type& timeout) {
        set_timeout(timeout.count());
    }

    void basic_async_service::set_timeout(duration_rep_type val) {
        invocation::with_void_return_value(_set_timeout, val);
    }

    // TODO: TBD: really, these should probably be more an effect of engaging the Socket with the AIO service.
    void basic_async_service::retain_message(binary_message* const bmp) const {
        if (!bmp) { return; }
        // Similarly with Socket send/receive, Message Cedes ownership to the AIO.
        invocation::with_void_return_value(_set_msg, bmp->cede_message());
    }

    void basic_async_service::cede_message(binary_message* const bmp) const {
        msg_type* msgp = nullptr;
        // Ditto re: Ceding/resuming ownership.
        invocation::with_result(_get_msg, &msgp);
        if (!msgp) { return; }
        if (bmp) {
            bmp->retain(msgp);
        }
        else {
            // This could be an unusual use case in which we should just free the message (probably).
            const auto& op = bind(&::nng_msg_free, msgp);
            invocation::with_void_return_value(op);
        }
    }
}
