#include "socket.h"
#include "dialer.h"
#include "listener.h"
#include "exceptions.hpp"

#include <vector>

namespace nng {

    socket::socket(const nng_ctor_func& nng_ctor)
        : sid(0)
        , sender()
        , receiver()
        , messenger()
        , options() {

        const auto errnum = nng_ctor(&sid);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    socket::~socket() {
        close();
    }

    // TODO: TBD: ditto ec handling...
    void socket::listen(const std::string& addr, flag_type flags) {
        const auto errnum = ::nng_listen(sid, addr.c_str(), nullptr, flags);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void socket::listen(const std::string& addr, listener* const lp, flag_type flags) {
        const auto errnum = ::nng_listen(sid, addr.c_str(), lp ? &(lp->lid) : nullptr, flags);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void socket::dial(const std::string& addr, flag_type flags) {
        const auto errnum = ::nng_dial(sid, addr.c_str(), nullptr, flags);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void socket::dial(const std::string& addr, dialer* const dp, flag_type flags) {
        const auto errnum = ::nng_dial(sid, addr.c_str(), dp ? &(dp->did) : nullptr, flags);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void socket::close() {
        if (sid) {
            auto errnum = ::nng_close(sid);
            THROW_NNG_EXCEPTION_EC(errnum);
            sid = 0;
        }
    }

    void socket::shutdown() {
        if (sid) {
            // TODO: TBD: I'm not sure "shutdown" is quite the same thing as "close". In other words, we may still operate on the ID.
            auto errnum = ::nng_shutdown(sid);
            THROW_NNG_EXCEPTION_EC(errnum);
        }
    }

    template<class Buffer_>
    int send(int id, const Buffer_& buf, std::size_t sz, flag_type flags) {
        const auto errnum = ::nng_send(id, (void*)&buf[0], sz, static_cast<int>(flags));
        THROW_NNG_EXCEPTION_EC(errnum);
        return errnum;
    }
    
    template<class Buffer_>
    int try_receive(int id, Buffer_& buf, std::size_t& sz, flag_type flags) {
        buf.resize(sz);
        const auto errnum = ::nng_recv(id, &buf[0], &sz, static_cast<int>(flags));
        THROW_NNG_EXCEPTION_EC(errnum);
        return errnum;
    }

    void socket::send(binary_message_type* const bmp, flag_type flags) {
        auto* msgp = bmp->get_msgp();
        const auto errnum = ::nng_sendmsg(sid, msgp, static_cast<int>(flags));
        THROW_NNG_EXCEPTION_EC(errnum);
        /* Yes, this is not a mistake. Message passing semantics means that NNG assumes
        ownership of the message after passing. Effectively, this nullifies the message. */
        bmp->set_msgp(nullptr);
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
            THROW_NNG_EXCEPTION_EC(errnum);
        }
        catch (...) {
            // TODO: TBD: this is probably (PROBABLY) about as good as we can expect here...
            if (msgp != nullptr) {
                ::nng_msg_free(msgp);
            }
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

    // Convenience option wrappers.
    void socket::set_option(const std::string& name, const std::string& val, option_size_type sz) {
        const auto errnum = ::nng_setopt(sid, name.c_str(), val.c_str(), sz);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void socket::set_option(const std::string& name, const std::string& val) {
        const auto errnum = ::nng_setopt(sid, name.c_str(), val.c_str(), val.length());
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void socket::get_option(const std::string& name, std::string& val, option_size_type& sz) {
        const auto errnum = ::nng_getopt(sid, name.c_str(), (void*)&val[0], &sz);
        THROW_NNG_EXCEPTION_EC(errnum);
        // Do this because the C++ buffer does not appear to be honored quite right when working with the C buffers.
        val.resize(sz - 1);
    }

    void socket::get_option(const std::string& name, std::string& val) {
        option_size_type sz = val.size();
        const auto errnum = ::nng_getopt(sid, name.c_str(), (void*)&val[0], &sz);
        THROW_NNG_EXCEPTION_EC(errnum);
        // Do this because the C++ buffer does not appear to be honored quite right when working with the C buffers.
        val.resize(sz - 1);
    }

    void socket::set_option(const std::string& name, const void* valp, option_size_type sz) {
        const auto errnum = ::nng_setopt(sid, name.c_str(), valp, sz);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void socket::get_option(const std::string& name, void* val, option_size_type* szp) {
        const auto errnum = ::nng_getopt(sid, name.c_str(), val, szp);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void socket::set_option_int(const std::string& name, int val) {
        const auto errnum = ::nng_setopt_int(sid, name.c_str(), val);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void socket::set_option_size(const std::string& name, option_size_type val) {
        const auto errnum = ::nng_setopt_size(sid, name.c_str(), val);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void socket::set_option_usec(const std::string& name, uint64_t val) {
        const auto errnum = ::nng_setopt_usec(sid, name.c_str(), val);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void socket::get_option_int(const std::string& name, int* valp) {
        const auto errnum = ::nng_getopt_int(sid, name.c_str(), valp);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void socket::get_option_size(const std::string& name, option_size_type* valp) {
        const auto errnum = ::nng_getopt_size(sid, name.c_str(), valp);
        THROW_NNG_EXCEPTION_EC(errnum);
    }

    void socket::get_option_usec(const std::string& name, uint64_t* valp) {
        const auto errnum = ::nng_getopt_usec(sid, name.c_str(), valp);
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
