#include "basic_async_service.h"
#include "../invocation.hpp"
#include "../../messaging/binary_message.h"

namespace nng {

    using std::bind;
    using std::placeholders::_1;
    using nng::exceptions::invalid_operation;

    _BasicAsyncService::_BasicAsyncService()
        : basic_async_service([]() {}) {
    }

    _BasicAsyncService::_BasicAsyncService(const basic_callback_func& on_cb)
        : IHaveOne(), ICanClose(), IHaveOptions()
        , _aiop(nullptr), _on_cb()
        , _free(), _wait(), _stop(), _cancel()
        , _result(), _get_msg(), _set_msg() {

        start(on_cb);
    }

    _BasicAsyncService::~_BasicAsyncService() {
        free();
    }

    void _BasicAsyncService::free() {
        if (!HasOne()) { return; }
        invocation::with_void_return_value(_free);
        _aiop = nullptr;
    }

    void _BasicAsyncService::configure(aio_type* const aiop) {

        auto op = GetOptions();

        op->set_setters(
            bind(&::nng_aio_set_timeout, aiop, _1)
        );

        _free = bind(&::nng_aio_free, aiop);
        _wait = bind(&::nng_aio_wait, aiop);
        _stop = bind(&::nng_aio_stop, aiop);
        _cancel = bind(&::nng_aio_cancel, aiop);
        _result = bind(&::nng_aio_result, aiop);
        _get_msg = bind(&::nng_aio_get_msg, aiop);
        _set_msg = bind(&::nng_aio_set_msg, aiop, _1);
    }

    void _BasicAsyncService::_aoi_cb(void* selfp) {
        // There is not much we can do if it isn't "this".
        auto __selfp = static_cast<basic_async_service*>(selfp);
        if (!__selfp) throw invalid_operation("Invalid asynchronous callback");
        __selfp->_on_cb();
    }

    bool _BasicAsyncService::HasOne() const {
        return _aiop != nullptr;
    }

    void _BasicAsyncService::Close() {
        Close(false);
    }

    void _BasicAsyncService::Close(bool force) {
        if (force) { cancel(); }
        else { stop(); }
        // TODO: TBD: this is probably sufficient; may need to wait() however.
        free();
        configure(_aiop = nullptr);
    }

    void _BasicAsyncService::start(const basic_callback_func& on_cb) {
        // Calling this version of Start means we at least want to re-set the Callback.
        _on_cb = on_cb;
        start();
    }

    void _BasicAsyncService::start() {
        // This version of Start leaves the Callback intact, whatever was once upon a time.
        if (HasOne()) { return; }
        auto alloc = bind(&::nng_aio_alloc, _1, &basic_async_service::_aoi_cb, this);
        invocation::with_default_error_handling(alloc, &_aiop);
        configure(_aiop);
    }

    void _BasicAsyncService::wait() const {
        invocation::with_void_return_value(_wait);
    }
    
    void _BasicAsyncService::stop() const {
        invocation::with_void_return_value(_stop);
    }
    
    void _BasicAsyncService::cancel() const {
        invocation::with_void_return_value(_cancel);
    }

    bool _BasicAsyncService::success() const {
        invocation::with_default_error_handling(_result);
        return true;
    }

    bool _BasicAsyncService::try_success() const {
        try {
            return success();
        }
        catch (...) {
            // TODO: TBD: may log the exception here...
            return false;
        }
    }

    void _BasicAsyncService::timed_wait(const duration_type& timeout) {
        GetOptions()->SetTimeoutDuration(timeout);
        wait();
    }

    void _BasicAsyncService::timed_wait(duration_rep_type val) {
        GetOptions()->SetTimeoutMilliseconds(val);
        wait();
    }

    // TODO: TBD: really, these should probably be more an effect of engaging the Socket with the AIO service.
    void _BasicAsyncService::retain_message(_Message* const bmp) const {
        if (!bmp) { return; }
        // Similarly with Socket send/receive, Message Cedes ownership to the AIO.
        invocation::with_void_return_value(_set_msg, bmp->cede_message());
    }

    void _BasicAsyncService::cede_message(_Message* const bmp) const {
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
