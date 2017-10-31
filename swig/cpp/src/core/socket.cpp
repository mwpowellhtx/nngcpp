#include "socket.h"
#include "listener.h"
#include "dialer.h"
#include "invocation.hpp"

#include <vector>

namespace nng {

    using std::placeholders::_1;
    using std::placeholders::_2;
    using std::placeholders::_3;
    using std::bind;

    _Socket::_Socket(const nng_ctor_func& nng_ctor)
        : IHaveOne(), IProtocol(), ICanClose(), ICanListen()
        , ICanDial(), ISender(), IReceiver(), IHaveOptions()
        , sid(0) {

        invocation::with_default_error_handling(nng_ctor, &sid);
        configure_options(sid);
    }

    _Socket::~_Socket() {
        Close();
    }

    _Socket::options_type* const _Socket::GetOptions() {
        return have_options_type::GetOptions();
    }

    void _Socket::configure_options(nng_type sid) {

        // Informs the Protcol of its "getters".
        set_getters(
            bind(&::nng_protocol, sid)
            , bind(&::nng_peer, sid)
        );

        // As well as the Options API.
        auto op = GetOptions();

        op->set_getters(
            bind(&::nng_getopt, sid, _1, _2, _3)
            , bind(&::nng_getopt_int, sid, _1, _2)
            , bind(&::nng_getopt_size, sid, _1, _2)
            , bind(&::nng_getopt_ms, sid, _1, _2)
        );

        op->set_setters(
            bind(&::nng_setopt, sid, _1, _2, _3)
            , bind(&::nng_setopt_int, sid, _1, _2)
            , bind(&::nng_setopt_size, sid, _1, _2)
            , bind(&::nng_setopt_ms, sid, _1, _2)
        );
    }

    void _Socket::Close() {
        if (!HasOne()) { return; }
        // Close is its own operation apart from Shutdown.
        const auto op = bind(&::nng_close, sid);
        invocation::with_default_error_handling(op);
        // Closed is closed.
        configure_options(sid = 0);
    }

    void _Socket::shutdown() {
        // Shutdown is its own operation apart from Closed.
        const auto op = bind(&::nng_shutdown, sid);
        invocation::with_default_error_handling(op);
        // Which socket can still be in operation.
    }

    bool _Socket::HasOne() const {
        return sid > 0;
    }

    // TODO: TBD: ditto ec handling...
    void _Socket::Listen(const std::string& addr, flag_type flags) {
        const auto& op = bind(&::nng_listen, sid, _1, _2, _3);
        invocation::with_default_error_handling(op, addr.c_str(), nullptr, static_cast<int>(flags));
    }

    void _Socket::Listen(const std::string& addr, _Listener* const lp, flag_type flags) {
        const auto& op = bind(&::nng_listen, sid, _1, _2, _3);
        invocation::with_default_error_handling(op, addr.c_str()
            , lp ? &(lp->lid) : nullptr, static_cast<int>(flags));
        if (lp) { lp->on_listened(); }
    }

    void _Socket::Dial(const std::string& addr, flag_type flags) {
        const auto& op = bind(&::nng_dial, sid, _1, _2, _3);
        invocation::with_default_error_handling(op, addr.c_str(), nullptr, flags);
    }

    void _Socket::Dial(const std::string& addr, _Dialer* const dp, flag_type flags) {
        const auto& op = bind(&::nng_dial, sid, _1, _2, _3);
        invocation::with_default_error_handling(op, addr.c_str()
            , dp ? &(dp->did) : nullptr, static_cast<int>(flags));
        if (dp) { dp->on_dialed(); }
    }

    template<class Buffer_>
    void send(int sid, const Buffer_& buf, std::size_t sz, flag_type flags) {
        // &buf[0] ????
        const auto& op = bind(&::nng_send, sid, _1, _2, _3);
        invocation::with_default_error_handling(op, (void*)buf.data(), sz
            , static_cast<int>(flags));
    }
    
    void _Socket::Send(const buffer_vector_type* const bufp, flag_type flags) {
        nng::send(sid, *bufp, bufp->size(), flags);
    }

    void _Socket::Send(const buffer_vector_type* const bufp, size_type sz, flag_type flags) {
        nng::send(sid, *bufp, sz, flags);
    }

    void _Socket::Send(binary_message* const bmp, flag_type flags) {
        auto* msgp = bmp->cede_message();
        if (msgp == nullptr) { return; }
        const auto op = bind(&::nng_sendmsg, sid, msgp, _1);
        invocation::with_default_error_handling(op, static_cast<int>(flags));
    }

    void _Socket::SendAsync(const basic_async_service* const svcp) {
        const auto& op = bind(&::nng_send_aio, sid, svcp->_aiop);
        invocation::with_void_return_value(op);
    }

    template<class Buffer_>
    bool try_receive(int sid, Buffer_& buf, std::size_t& sz, flag_type flags) {
        buf.resize(sz);
        // &buf[0] ????
        const auto& op = bind(&::nng_recv, sid, _1, _2, _3);
        invocation::with_default_error_handling(op, (void*)buf.data(), &sz
            , static_cast<int>(flags));
        return sz > 0;
    }

    std::unique_ptr<binary_message> _Socket::Receive(flag_type flags) {
        // TODO: TBD: why return unique pointer here? Why not return the object itself? I know *why*, it is "new memory", just is it necessary? especially with SWIG API comprehension?
        auto bmup = std::make_unique<binary_message>();
        TryReceive(bmup.get(), flags);
        return bmup;
    }

    bool _Socket::TryReceive(binary_message* const bmp, flag_type flags) {
        /* So this is somewhat of a long way around, but it represents the cost of NNG message
        ownership semantics. The cost has to be paid at some point, either on the front side or
        the back side, so we pay for it here in additional semantics. */
        msg_type* msgp = nullptr;
        try {
            const auto& recv_ = bind(&::nng_recvmsg, sid, &msgp, _1);
            invocation::with_default_error_handling(recv_, static_cast<int>(flags));
        }
#if 0
        catch (std::exception& ex) {
#else
        catch (...) {
#endif
            // TODO: TBD: this is probably (PROBABLY) about as good as we can expect here...
            if (msgp) {
                const auto& free_ = bind(&::nng_msg_free, msgp);
                invocation::with_void_return_value(free_);
            }
            // Re-throw the exception after taking care of potential memory allocation.
            throw;
        }
        bmp->retain(msgp);
        return bmp->HasOne();
    }

    buffer_vector_type _Socket::Receive(size_type& sz, flag_type flags) {
        buffer_vector_type buf;
        // TODO: TBD: if we do not or cannot receive anything, return whatever we do have?
        _Socket::TryReceive(&buf, sz, flags);
        return buf;
    }

    bool _Socket::TryReceive(buffer_vector_type* const bufp, size_type& sz, flag_type flags) {
        return nng::try_receive(sid, *bufp, sz, flags);
    }

    void _Socket::ReceiveAsync(basic_async_service* const svcp) {
        const auto& op = bind(&::nng_recv_aio, sid, svcp->_aiop);
        invocation::with_void_return_value(op);
    }
}
