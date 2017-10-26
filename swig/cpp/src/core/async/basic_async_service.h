#ifndef NNGCPP_BASIC_ASYNC_SERVICE_H
#define NNGCPP_BASIC_ASYNC_SERVICE_H

#define NNG_ONLY
#include <nngcpp.h>

#include "../types.h"

#include <functional>

namespace nng {

#ifndef NNGCPP_SOCKET_H
    class socket;
#endif // NNGCPP_SOCKET_H

#ifndef NNGCPP_BINARY_MESSAGE_H
    class binary_message;
#endif // NNGCPP_BINARY_MESSAGE_H

    class basic_async_service {

        friend class socket;

        typedef ::nng_aio aio_type;

        aio_type* _aiop;

        static void _aoi_cb(void* selfp);

        typedef std::function<void()> void_noarg_func;
        typedef std::function<int()> int_noarg_func;
        typedef std::function<void(duration_rep_type)> set_timeout_func;

        typedef std::function<msg_type*()> msg_ptr_func;
        typedef std::function<void(msg_type*)> void_ptr_msg_func;

        void_noarg_func _free;
        void_noarg_func _wait;
        void_noarg_func _stop;
        void_noarg_func _cancel;
        int_noarg_func _result;
        set_timeout_func _set_timeout;

        msg_ptr_func _get_msg;
        void_ptr_msg_func _set_msg;

        void configure(aio_type* const aiop);

    public:

        /* TODO: TBD: for the time being, callbacks from the Async service are parameterless and return
        nothing. That could change downstream from here, but for now it's K.I.S.S. as well as Y.A.G.N.I.. */
        typedef std::function<void()> basic_callback_func;

        basic_async_service();

        basic_async_service(const basic_callback_func& on_cb);

        virtual ~basic_async_service();

        virtual void wait() const;

        virtual void stop() const;

        virtual void cancel() const;

        virtual bool success() const;

        virtual bool try_success() const;

        virtual void set(const duration_type& timeout);

        virtual void set_timeout(duration_rep_type val);

        virtual void retain_message(binary_message* const bmp) const;

        virtual void cede_message(binary_message* const bmp) const;

    private:

        basic_callback_func _on_cb;
    };
}

#endif // NNGCPP_BASIC_ASYNC_SERVICE_H
