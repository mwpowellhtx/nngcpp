#include "socket.h"
#include "messenger.h"
#include "listener.h"
#include "dialer.h"
#include "options.h"
#include "exceptions.hpp"

#include <vector>

namespace nng {

    using std::placeholders::_1;
    using std::placeholders::_2;
    using std::placeholders::_3;
    using std::placeholders::_4;

    socket::socket(const nng_ctor_func& nng_ctor)
        : sid(0)
        , sender()
        , receiver()
        , messenger()
        , options_reader()
        , options_writer() {

        const auto errnum = nng_ctor(&sid);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    socket::~socket() {
        close();
    }

    void socket::close() {
        if (sid) {
            // Close is its own operation apart from Shutdown.
            const auto op = std::bind(&::nng_close, _1);
            const auto errnum = op(sid);
            THROW_NNG_EXCEPTION_IF_NOT_ONEOF(errnum, ec_eunknown, ec_enone);
            // Closed is closed.
            sid = 0;
        }
    }

    void socket::shutdown() {
        // Shutdown is its own operation apart from Closed.
        const auto op = std::bind(&::nng_shutdown, _1);
        const auto errnum = op(sid);
        THROW_NNG_EXCEPTION_IF_NOT_ONEOF(errnum, ec_eunknown, ec_enone);
        // Which socket can still be in operation.
    }

    bool socket::is_open() const {
        return sid != 0;
    }

    // TODO: TBD: ditto ec handling...
    void socket::listen(const std::string& addr, flag_type flags) {
        const auto& op = std::bind(&::nng_listen, _1, _2, _3, _4);
        const auto errnum = op(sid, addr.c_str(), nullptr, static_cast<int>(flags));
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void socket::listen(const std::string& addr, listener* const lp, flag_type flags) {
        const auto& op = std::bind(&::nng_listen, _1, _2, _3, _4);
        const auto errnum = op(sid, addr.c_str(), lp ? &(lp->lid) : nullptr, static_cast<int>(flags));
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void socket::dial(const std::string& addr, flag_type flags) {
        const auto& op = std::bind(&::nng_dial, _1, _2, _3, _4);
        const auto errnum = op(sid, addr.c_str(), nullptr, flags);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void socket::dial(const std::string& addr, dialer* const dp, flag_type flags) {
        const auto& op = std::bind(&::nng_dial, _1, _2, _3, _4);
        const auto errnum = op(sid, addr.c_str(), dp ? &(dp->did) : nullptr, static_cast<int>(flags));
        THROW_NNG_EXCEPTION_EC(errnum);
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
        const auto op = std::bind(&::nng_sendmsg, _1, _2, _3);
        const auto errnum = op(sid, msgp, static_cast<int>(flags));
        /* Yes, this is not a mistake. Message passing semantics means that NNG assumes
        ownership of the message after passing. Effectively, this nullifies the message. */
        // TODO: TBD: at least for now this seems reasonable: depending on the error code, we may or may not want to do this...
        if (!errnum) { bmp->on_sent(); }
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    int socket::send(const buffer_vector_type* const bufp, flag_type flags) {
        return nng::send(sid, *bufp, bufp->size(), flags);
    }

    int socket::send(const buffer_vector_type* const bufp, nng::size_type sz, flag_type flags) {
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

    socket::buffer_vector_type socket::receive(nng::size_type& sz, flag_type flags) {
        buffer_vector_type buf;
        try_receive(&buf, sz, flags);
        return buf;
    }

    int socket::try_receive(buffer_vector_type* const bufp, nng::size_type& sz, flag_type flags) {
        return nng::try_receive(sid, *bufp, sz, flags);
    }

    // Convenience option wrappers.
    void socket::get_option(const std::string& name, void* val, nng::size_type* szp) {
        const auto op = std::bind(&::nng_getopt, _1, _2, _3, _4);
        const auto errnum = op(sid, name.c_str(), val, szp);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void socket::get_option(const std::string& name, std::string& val) {
        const auto op = std::bind(&::nng_getopt, _1, _2, _3, _4);
        size_type sz = val.size();
        const auto errnum = op(sid, name.c_str(), (void*)&val[0], &sz);
        THROW_NNG_EXCEPTION_EC(errnum);
        // Accounting for C style strings during transmission.
        if (sz > 0) { val.resize(sz - 1); }
    }

    void socket::get_option(const std::string& name, std::string& val, size_type& sz) {
        val.resize(sz);
        get_option(name, val);
    }

    void socket::get_option_int(const std::string& name, int* valp) {
        const auto op = std::bind(&::nng_getopt_int, _1, _2, _3);
        const auto errnum = op(sid, name.c_str(), valp);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void socket::get_option_size(const std::string& name, nng::size_type* valp) {
        const auto op = std::bind(&::nng_getopt_size, _1, _2, _3);
        const auto errnum = op(sid, name.c_str(), valp);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void socket::get_option(const std::string& name, duration_type* valp) {
        duration_type::rep val;
        get_option_ms(name, &val);
        *valp = duration_type(val);
    }

    void socket::get_option_ms(const std::string& name, duration_rep_type* valp) {
        const auto op = std::bind(&::nng_getopt_ms, _1, _2, _3);
        const auto errnum = op(sid, name.c_str(), valp);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void socket::set_option(const std::string& name, const void* valp, nng::size_type sz) {
        const auto op = std::bind(&::nng_setopt, _1, _2, _3, _4);
        const auto errnum = op(sid, name.c_str(), valp, sz);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void socket::set_option(const std::string& name, const std::string& val, nng::size_type sz) {
        const auto op = std::bind(&::nng_setopt, _1, _2, _3, _4);
        const auto errnum = ::nng_setopt(sid, name.c_str(), val.c_str(), sz);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void socket::set_option(const std::string& name, const std::string& val) {
        const auto op = std::bind(&::nng_setopt, _1, _2, _3, _4);
        const auto errnum = ::nng_setopt(sid, name.c_str(), val.c_str(), val.length());
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void socket::set_option_int(const std::string& name, int val) {
        const auto op = std::bind(&::nng_setopt_int, _1, _2, _3);
        const auto errnum = op(sid, name.c_str(), val);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void socket::set_option_size(const std::string& name, nng::size_type val) {
        const auto op = std::bind(&::nng_setopt_size, _1, _2, _3);
        const auto errnum = op(sid, name.c_str(), val);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void socket::set_option(const std::string& name, duration_type val) {
        set_option_ms(name, val.count());
    }

    void socket::set_option_ms(const std::string& name, duration_rep_type val) {
        const auto op = std::bind(&::nng_setopt_ms, _1, _2, _3);
        const auto errnum = op(sid, name.c_str(), val);
        THROW_NNG_EXCEPTION_EC(errnum);
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
