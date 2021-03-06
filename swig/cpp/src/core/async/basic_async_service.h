#ifndef NNGCPP_BASIC_ASYNC_SERVICE_H
#define NNGCPP_BASIC_ASYNC_SERVICE_H

#define NNG_ONLY
#include <nngcpp.h>

#include "../types.h"

#include "../IHaveOne.hpp"
#include "../ICanClose.hpp"

#include "async_writer.h"
#include "../../options/IHaveOptions.hpp"

#include <functional>

namespace nng {

#ifndef NNGCPP_SOCKET_H
    class _Socket;
#endif // NNGCPP_SOCKET_H

#ifndef NNGCPP_BINARY_MESSAGE_H
    class _Message;
#endif // NNGCPP_BINARY_MESSAGE_H

    class _BasicAsyncService
        : public IHaveOne
        , public ICanClose
        , public IHaveOptions<_AsyncOptionWriter> {

        friend class _Socket;

        typedef ::nng_aio aio_type;

        aio_type* _aiop;

        static void _aoi_cb(void* selfp);

        typedef std::function<void()> void_noarg_func;
        typedef std::function<int()> int_noarg_func;

        typedef std::function<msg_type*()> msg_ptr_func;
        typedef std::function<void(msg_type*)> void_ptr_msg_func;

        void_noarg_func _free;
        void_noarg_func _wait;
        void_noarg_func _stop;
        void_noarg_func _cancel;
        int_noarg_func _result;

        msg_ptr_func _get_msg;
        void_ptr_msg_func _set_msg;

        void configure(aio_type* const aiop);

    public:

        /* TODO: TBD: for the time being, callbacks from the Async service are parameterless and return
        nothing. That could change downstream from here, but for now it's K.I.S.S. as well as Y.A.G.N.I.. */
        typedef std::function<void()> basic_callback_func;

        _BasicAsyncService();

        _BasicAsyncService(const basic_callback_func& on_cb);

        virtual ~_BasicAsyncService();

        virtual bool HasOne() const override;

        virtual void Close() override;

        virtual void Close(bool force);

        virtual void Start(const basic_callback_func& on_cb);

        virtual void Start();

        virtual void Wait() const;

        virtual void Stop() const;

        virtual void Cancel() const;

        virtual bool Success() const;

        virtual bool TrySuccess() const;

        virtual void TimedWait(const duration_type& timeout);

        virtual void TimedWait(duration_rep_type val);

        virtual void Retain(_Message& m) const;

        virtual void Cede(_Message& m) const;

    private:

        basic_callback_func _on_cb;

        void free();
    };

    typedef _BasicAsyncService basic_async_service;

    _Message& operator<<(_Message& m, _BasicAsyncService& svc);
    _Message& operator >> (_BasicAsyncService& svc, _Message& m);

    _BasicAsyncService& operator<<(_BasicAsyncService& svc, _Message& m);
    _BasicAsyncService& operator >> (_Message& m, _BasicAsyncService& svc);
}

#endif // NNGCPP_BASIC_ASYNC_SERVICE_H
