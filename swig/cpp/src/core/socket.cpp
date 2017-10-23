#include "socket.h"
#include "messenger.h"
#include "listener.h"
#include "dialer.h"
#include "exceptions.hpp"

#include <vector>

namespace nng {

    using std::placeholders::_1;
    using std::placeholders::_2;
    using std::placeholders::_3;
    using std::bind;

    socket::socket(const nng_ctor_func& nng_ctor)
        : sender(), receiver(), messenger()
        , sid(0), _options() {

        const auto errnum = nng_ctor(&sid);
        THROW_NNG_EXCEPTION_EC(errnum);

        configure_options(sid);
    }

    socket::~socket() {
        close();
    }

    void socket::configure_options(nng_type sid) {

        _options.set_getters(
            bind(&::nng_getopt, sid, _1, _2, _3)
            , bind(::nng_getopt_int, sid, _1, _2)
            , bind(::nng_getopt_size, sid, _1, _2)
            , bind(&::nng_getopt_ms, sid, _1, _2)
        );

        _options.set_setters(
            bind(&::nng_setopt, sid, _1, _2, _3)
            , bind(::nng_setopt_int, sid, _1, _2)
            , bind(::nng_setopt_size, sid, _1, _2)
            , bind(&::nng_setopt_ms, sid, _1, _2)
        );
    }

    void socket::close() {
        if (!has_one()) { return; }
        // Close is its own operation apart from Shutdown.
        const auto op = bind(&::nng_close, sid);
        const auto errnum = op();
        THROW_NNG_EXCEPTION_IF_NOT_ONEOF(errnum, ec_eunknown, ec_enone);
        // Closed is closed.
        configure_options(sid = 0);
    }

    void socket::shutdown() {
        // Shutdown is its own operation apart from Closed.
        const auto op = bind(&::nng_shutdown, sid);
        const auto errnum = op();
        THROW_NNG_EXCEPTION_IF_NOT_ONEOF(errnum, ec_eunknown, ec_enone);
        // Which socket can still be in operation.
    }

    bool socket::has_one() const {
        return sid > 0;
    }

    // TODO: TBD: ditto ec handling...
    void socket::listen(const std::string& addr, flag_type flags) {
        const auto& op = bind(&::nng_listen, sid, _1, _2, _3);
        const auto errnum = op(addr.c_str(), nullptr, static_cast<int>(flags));
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void socket::listen(const std::string& addr, listener* const lp, flag_type flags) {
        const auto& op = bind(&::nng_listen, sid, _1, _2, _3);
        const auto errnum = op(addr.c_str(), lp ? &(lp->lid) : nullptr, static_cast<int>(flags));
        THROW_NNG_EXCEPTION_EC(errnum);
        if (lp) { lp->on_listened(); }
    }

    void socket::dial(const std::string& addr, flag_type flags) {
        const auto& op = bind(&::nng_dial, sid, _1, _2, _3);
        const auto errnum = op(addr.c_str(), nullptr, flags);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void socket::dial(const std::string& addr, dialer* const dp, flag_type flags) {
        const auto& op = bind(&::nng_dial, sid, _1, _2, _3);
        const auto errnum = op(addr.c_str(), dp ? &(dp->did) : nullptr, static_cast<int>(flags));
        THROW_NNG_EXCEPTION_EC(errnum);
        if (dp) { dp->on_dialed(); }
    }

    template<class Buffer_>
    int send(int id, const Buffer_& buf, std::size_t sz, flag_type flags) {
        // &buf[0] ????
        const auto errnum = ::nng_send(id, (void*)buf.data(), sz, static_cast<int>(flags));
        THROW_NNG_EXCEPTION_EC(errnum);
        return errnum;
    }
    
    template<class Buffer_>
    int try_receive(int id, Buffer_& buf, std::size_t& sz, flag_type flags) {
        buf.resize(sz);
        // &buf[0] ????
        const auto errnum = ::nng_recv(id, (void*)buf.data(), &sz, static_cast<int>(flags));
        THROW_NNG_EXCEPTION_EC(errnum);
        return errnum;
    }

    void socket::send(binary_message_type* const bmp, flag_type flags) {
        auto* msgp = bmp->get_msgp();
        const auto op = bind(&::nng_sendmsg, sid, msgp, _1);
        const auto errnum = op(static_cast<int>(flags));
        /* Yes, this is not a mistake. Message passing semantics means that NNG assumes
        ownership of the message after passing. Effectively, this nullifies the message. */
        // TODO: TBD: at least for now this seems reasonable: depending on the error code, we may or may not want to do this...
        if (!errnum) { bmp->on_sent(); }
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    int socket::send(const buffer_vector_type* const bufp, flag_type flags) {
        return nng::send(sid, *bufp, bufp->size(), flags);
    }

    int socket::send(const buffer_vector_type* const bufp, size_type sz, flag_type flags) {
        return nng::send(sid, *bufp, sz, flags);
    }

    std::unique_ptr<socket::binary_message_type> socket::receive(flag_type flags) {
        auto bmup = std::make_unique<binary_message_type>();
        try_receive(bmup.get(), flags);
        return bmup;
    }

    int socket::try_receive(binary_message_type* const bmp, flag_type flags) {
        /* So this is somewhat of a long way around, but it represents the cost of NNG message
        ownership semantics. The cost has to be paid at some point, either on the front side or
        the back side, so we pay for it here in additional semantics. */
        ::nng_msg* msgp = nullptr;
        const auto errnum = ::nng_recvmsg(sid, &msgp, static_cast<int>(flags));
        try {
            // Which we actually do want to set this one.
            THROW_NNG_EXCEPTION_EC(errnum);
        }
        catch (...) {
            // TODO: TBD: this is probably (PROBABLY) about as good as we can expect here...
            if (msgp) { ::nng_msg_free(msgp); }
            // Re-throw the exception after taking care of potential memory allocation.
            throw;
        }
        bmp->set_msgp(msgp);
        return errnum;
    }

    socket::buffer_vector_type socket::receive(size_type& sz, flag_type flags) {
        buffer_vector_type buf;
        try_receive(&buf, sz, flags);
        return buf;
    }

    int socket::try_receive(buffer_vector_type* const bufp, size_type& sz, flag_type flags) {
        return nng::try_receive(sid, *bufp, sz, flags);
    }

    options_reader_writer* const socket::options() {
        return &_options;
    }

    protocol_type to_protocol_type(int value) {
        return static_cast<protocol_type>(value);
    }

    int to_int(const protocol_type value) {
        return static_cast<int>(value);
    }

    protocol_type socket::get_protocol() const {
        return to_protocol_type(::nng_protocol(sid));
    }

    protocol_type socket::get_peer() const {
        return to_protocol_type(::nng_peer(sid));
    }

    bool socket::is_protocol_configured() const {
        return get_protocol() != protocol_none;
    }

    bool socket::is_peer_configured() const {
        return get_peer() != protocol_none;
    }
}
