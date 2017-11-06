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

        Start(on_cb);
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
        if (force) { Cancel(); }
        else { Stop(); }
        // TODO: TBD: this is probably sufficient; may need to wait() however.
        free();
        configure(_aiop = nullptr);
    }

    void _BasicAsyncService::Start(const basic_callback_func& on_cb) {
        // Calling this version of Start means we at least want to re-set the Callback.
        _on_cb = on_cb;
        Start();
    }

    void _BasicAsyncService::Start() {
        // This version of Start leaves the Callback intact, whatever was once upon a time.
        if (HasOne()) { return; }
        auto alloc = bind(&::nng_aio_alloc, _1, &basic_async_service::_aoi_cb, this);
        invocation::with_default_error_handling(alloc, &_aiop);
        configure(_aiop);
    }

    void _BasicAsyncService::Wait() const {
        invocation::with_void_return_value(_wait);
    }
    
    void _BasicAsyncService::Stop() const {
        invocation::with_void_return_value(_stop);
    }
    
    void _BasicAsyncService::Cancel() const {
        invocation::with_void_return_value(_cancel);
    }

    bool _BasicAsyncService::Success() const {
        invocation::with_default_error_handling(_result);
        return true;
    }

    bool _BasicAsyncService::TrySuccess() const {
        try {
            return Success();
        }
        catch (...) {
            // TODO: TBD: may log the exception here...
            return false;
        }
    }

    void _BasicAsyncService::TimedWait(const duration_type& timeout) {
        GetOptions()->SetTimeoutDuration(timeout);
        Wait();
    }

    void _BasicAsyncService::TimedWait(duration_rep_type val) {
        GetOptions()->SetTimeoutMilliseconds(val);
        Wait();
    }

    // TODO: TBD: really, these should probably be more an effect of engaging the Socket with the AIO service.
    void _BasicAsyncService::Retain(_Message& m) const {
        // Similarly with Socket send/receive, Message Cedes ownership to the AIO.
        invocation::with_void_return_value(_set_msg, m.cede_message());
    }

    void _BasicAsyncService::Cede(_Message& m) const {
        msg_type* msgp = nullptr;
        // Ditto re: Ceding/resuming ownership.
        invocation::with_result(_get_msg, &msgp);
        if (!msgp) { return; }
        m.retain(msgp);
    }

    _Message& operator<<(_Message& m, _BasicAsyncService& svc) {
        svc.Cede(m);
        return m;
    }

    _Message& operator >> (_BasicAsyncService& svc, _Message& m) {
        svc.Cede(m);
        return m;
    }

    _BasicAsyncService& operator<<(_BasicAsyncService& svc, _Message& m) {
        svc.Retain(m);
        return svc;
    }

    _BasicAsyncService& operator >> (_Message& m, _BasicAsyncService& svc) {
        svc.Retain(m);
        return svc;
    }
}
